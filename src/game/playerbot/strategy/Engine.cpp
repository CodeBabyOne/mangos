#include "../../pchdef.h"
#include "../playerbot.h"
#include "GenericActions.h"

#include "Engine.h"

using namespace ai;
using namespace std;

bool ActionExecutionListeners::ActionExecuted(Action* action)
{
    bool result = true;
    for (std::list<ActionExecutionListener*>::iterator i = listeners.begin(); i!=listeners.end(); i++)
    {
        result &= (*i)->ActionExecuted(action);
    }
    return result;
}

ActionExecutionListeners::~ActionExecutionListeners()
{
    for (std::list<ActionExecutionListener*>::iterator i = listeners.begin(); i!=listeners.end(); i++)
    {
        delete *i;
    }
    listeners.clear();
}


Engine::~Engine(void)
{
    Reset();
    clearStrategies();
    if (actionFactory)
        delete actionFactory;
}

void Engine::Reset()
{
    ActionNode* action = NULL;
    do 
    {
        action = queue.Pop();
    } while (action);

    for (std::list<TriggerNode*>::iterator i = triggers.begin(); i != triggers.end(); i++)
    {
        TriggerNode* trigger = *i;
        delete trigger;
    }
    triggers.clear();

    for (std::list<Multiplier*>::iterator i = multipliers.begin(); i != multipliers.end(); i++)
    {
        Multiplier* multiplier = *i;
        delete multiplier;
    }
    multipliers.clear();
}

void Engine::clearStrategies()
{
    for (std::list<Strategy*>::iterator i = strategies.begin(); i != strategies.end(); i++)
    {
        Strategy* strategy = *i;
        delete strategy;
    }
    strategies.clear();
}

void Engine::Init()
{
    Reset();

    for (std::list<Strategy*>::iterator i = strategies.begin(); i != strategies.end(); i++)
    {
        Strategy* strategy = *i;
        strategy->InitMultipliers(multipliers);
        strategy->InitTriggers(triggers);
        MultiplyAndPush(strategy->getDefaultActions());
    }
}


bool Engine::DoNextAction(Unit* unit, int depth) 
{
    bool actionExecuted = false;
    ActionBasket* basket = NULL;

    time_t currentTime = time(0);
    ProcessTriggers();

    int iterations = 0;
    do {
        basket = queue.Peek();
        if (basket) {
			if (++iterations > maxIterations)
				break;

            float relevance = basket->getRelevance(); // just for reference
            bool skipPrerequisites = basket->isSkipPrerequisites();
            ActionNode* actionNode = queue.Pop();
            Action* action = InitializeAction(actionNode);

            if (action && action->isUseful()) {
                if (action->isPossible()) {
                    if ((!skipPrerequisites || lastRelevance-relevance > 0.02) && 
                            MultiplyAndPush(actionNode->getPrerequisites(), relevance + 0.01)) {
                        PushAgain(actionNode, relevance);
                        continue;
                    }

                    sLog.outDetail("A:%s", action->getName());

                    if (actionExecutionListeners.ActionExecuted(actionNode->getAction()))
                        actionExecuted = action->ExecuteResult();

                    if (actionExecuted) {
                        MultiplyAndPush(actionNode->getContinuers());
                        lastRelevance = relevance;
                        delete actionNode;
                        break;
                    }
                    else {
                        MultiplyAndPush(actionNode->getAlternatives(), relevance);
                        sLog.outDetail("NOT EXECUTED:%s", actionNode->getName());
                    }
                }
                else {
                    MultiplyAndPush(actionNode->getAlternatives(), relevance);
                    sLog.outDetail("IMPOSSIBLE:%s", actionNode->getName());
                }
            }
            else {
                lastRelevance = relevance;
                sLog.outDetail("USELESS:%s", actionNode->getName());
            }
            delete actionNode;
        }
    }
    while (basket);

    if (!basket) {
        lastRelevance = 0.0f;
        PushDefaultActions();
        if (queue.Peek() && depth < 2)
            return DoNextAction(unit, depth + 1);
    }

    if (time(0) - currentTime > 1) {
        sLog.outDetail("too long execution");
    }

    return actionExecuted;
}

ActionNode* Engine::createAction(const char* name)
{
    for (std::list<Strategy*>::iterator i = strategies.begin(); i != strategies.end(); i++)
    {
        Strategy* strategy = *i;
        ActionNode* node = strategy->createAction(name);
        if (node)
            return node;
    }
    return NULL;
}

bool Engine::MultiplyAndPush(NextAction** actions, float forceRelevance, bool skipPrerequisites)
{
    bool pushed = false;
    if (actions)
    {
        for (int j=0; j<10; j++) // TODO: remove 10
        {
            NextAction* nextAction = actions[j];
            if (nextAction)
            {
                ActionNode* action = createAction(nextAction->getName());
                InitializeAction(action);

                float k = nextAction->getRelevance();
                for (std::list<Multiplier*>::iterator i = multipliers.begin(); i!= multipliers.end(); i++)
                {
                    Multiplier* multiplier = *i;
                    k *= multiplier->GetValue(action->getAction());
                }

                if (forceRelevance > 0.0f)
                    k = forceRelevance;

                if (k > 0)
                {
                    queue.Push(new ActionBasket(action, k, skipPrerequisites));
                    pushed = true;
                }

                delete nextAction;
            }
            else 
                break;
        }
        delete actions;
    }
    return pushed;
}

bool Engine::ExecuteAction(const char* name)
{
	bool result = false;

    ActionNode *actionNode = createAction(name);
    Action* action = InitializeAction(actionNode);
    if (action && action->isPossible() && action->isUseful())
    {
        if (actionExecutionListeners.ActionExecuted(action))
		{
            result = action->ExecuteResult();
		}

        MultiplyAndPush(action->getContinuers());
        delete action;
    }
	return result;
}

void Engine::addStrategy(const char* name)
{
    removeStrategy(name);

    Strategy* strategy = actionFactory->createStrategy(name);
    if (strategy)
	{
        strategies.push_back(strategy);

		AiStrategyManager* manager = ai->GetStrategyManager();
		if (manager)
		{
			string list = strategy->GetIncompatibleStrategies();
			ai->GetStrategyManager()->ChangeStrategy(list.c_str(), this);
		}
	}

    Init();
}

void Engine::addStrategies(const char* first, ...)
{
	addStrategy(first);

	va_list vl;
	va_start(vl, first);

	const char* cur = NULL;
	do 
	{
		cur = va_arg(vl, const char*);
		if (cur)
			addStrategy(cur);
	}
	while (cur);

	va_end(vl);
}

bool Engine::removeStrategy(const char* name)
{
    for (std::list<Strategy*>::iterator i = strategies.begin(); i != strategies.end(); i++)
    {
        Strategy* strategy = *i;
        if (!strcmp(strategy->getName(), name))
        {
            strategies.remove(strategy);
            delete strategy;
            Init();
            return true;
        }
    }

    return false;
}

void Engine::toggleStrategy(const char* name)
{
    if (!removeStrategy(name)) 
        addStrategy(name);
}

void Engine::ProcessTriggers()
{
    for (std::list<TriggerNode*>::iterator i = triggers.begin(); i != triggers.end(); i++)
    {
        TriggerNode* node = *i;
        Trigger* trigger = node->getTrigger();
        if (!trigger)
        {
            trigger = actionFactory->createTrigger(node->getName());
            node->setTrigger(trigger);
        }

        if ((testMode || trigger->needCheck()) && trigger->IsActive())
        {
            MultiplyAndPush(node->getHandlers());
        }
    }
}

void Engine::PushDefaultActions() 
{
    for (std::list<Strategy*>::iterator i = strategies.begin(); i != strategies.end(); i++)
    {
        Strategy* strategy = *i;
        MultiplyAndPush(strategy->getDefaultActions());
    }
}

string Engine::ListStrategies()
{
    string s = "Strategies: ";

    if (strategies.empty())
        return s;

    for (std::list<Strategy*>::iterator i = strategies.begin(); i != strategies.end(); i++)
    {
        Strategy* strategy = *i;
        s.append(strategy->getName());
        s.append(", ");
    }
    return s.substr(0, s.length() - 2);
}

void Engine::PushAgain( ActionNode* actionNode, float relevance ) 
{
    NextAction** nextAction = new NextAction*[2];
    nextAction[0] = new NextAction(actionNode->getName(), relevance);
    nextAction[1] = NULL;
    MultiplyAndPush(nextAction, relevance, true);
    delete actionNode;
}

bool Engine::ContainsStrategy(StrategyType type)
{
	for (std::list<Strategy*>::iterator i = strategies.begin(); i != strategies.end(); i++)
	{
		Strategy* strategy = *i;
		if (strategy->GetType() & type)
			return true;
	}
	return false;
}

Action* Engine::InitializeAction(ActionNode* actionNode) 
{
    Action* action = actionNode->getAction();
    if (!action)
    {
        action = actionFactory->createAction(actionNode->getName());
        actionNode->setAction(action);
    }
    return action;
}
