#include "../../pchdef.h"
#include "../playerbot.h"

using namespace ai;
using namespace std;

void AiQuestManager::UpdateQuestNeedItems()
{
	questNeedItems.clear();

	for( QuestStatusMap::iterator iter=bot->getQuestStatusMap().begin(); iter!=bot->getQuestStatusMap().end(); ++iter )
	{
		const Quest *qInfo = sObjectMgr.GetQuestTemplate( iter->first );
		if( !qInfo )
			continue;

		QuestStatusData *qData = &iter->second;
		if( qData->m_status != QUEST_STATUS_INCOMPLETE )
			continue;

		for( int i=0; i<QUEST_OBJECTIVES_COUNT; i++ )
		{
			if( !qInfo->ReqItemCount[i] || (qInfo->ReqItemCount[i]-qData->m_itemcount[i])<=0 )
				continue;
			questNeedItems[qInfo->ReqItemId[i]] = (qInfo->ReqItemCount[i]-qData->m_itemcount[i]);
		}
	}
}

void AiQuestManager::ListQuests(QuestListFilter filter)
{
	AiSocialManager* socialManager = aiRegistry->GetSocialManager();

	bool showIncompleted = filter & QUEST_LIST_FILTER_INCOMPLETED;
	bool showCompleted = filter & QUEST_LIST_FILTER_COMPLETED;

	if (showIncompleted)
		socialManager->TellMaster("--- incomplete quests ---");
	int incompleteCount = ListQuests(false, !showIncompleted);

	if (showCompleted)
		socialManager->TellMaster("--- complete quests ---");
	int completeCount = ListQuests(true, !showCompleted);

	socialManager->TellMaster("--- summary ---");
	std::ostringstream out;
	out << "Total: " << (completeCount + incompleteCount) << " / 25 (incomplete: " << incompleteCount << ", complete: " << completeCount << ")";
	socialManager->TellMaster(out.str().c_str());
}

int AiQuestManager::ListQuests(bool completed, bool silent)
{
	int count = 0;
	for (uint16 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
	{
		uint32 questId = bot->GetQuestSlotQuestId(slot);
		if (!questId)
			continue;

		Quest const* pQuest = sObjectMgr.GetQuestTemplate(questId);
		bool isCompletedQuest = bot->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE;
		if (completed != isCompletedQuest)
			continue;
		
		count++;
		
		if (silent)
			continue;

		std::ostringstream stream;
		stream << " |cFFFFFF00|Hquest:" << questId << ':' << pQuest->GetQuestLevel() << "|h[" << pQuest->GetTitle() << "]|h|r";
		aiRegistry->GetSocialManager()->TellMaster(stream.str().c_str());
	}

	return count;
}

void AiQuestManager::DropQuest(const char* link)
{
	Player* master = ai->GetMaster();
	ObjectGuid oldSelection = master->GetSelectionGuid();
	master->SetSelectionGuid(bot->GetObjectGuid());
	
	PlayerbotChatHandler ch(bot);
	if (!ch.dropQuest(link))
	{
		aiRegistry->GetSocialManager()->TellMaster("Could not drop quest: ");
		aiRegistry->GetSocialManager()->TellMaster(link);
	}

	master->SetSelectionGuid(oldSelection);
}

void AiQuestManager::QuestLocalization(std::string& questTitle, const uint32 questID)
{
	int loc = ai->GetMaster()->GetSession()->GetSessionDbLocaleIndex();
	std::wstring wnamepart;

	QuestLocale const *pQuestInfo = sObjectMgr.GetQuestLocale(questID);
	if (pQuestInfo)
	{
		if (pQuestInfo->Title.size() > loc && !pQuestInfo->Title[loc].empty())
		{
			const std::string title = pQuestInfo->Title[loc];
			if (Utf8FitTo(title, wnamepart))
				questTitle = title.c_str();
		}
	}
}

void AiQuestManager::TurnInQuests( WorldObject *questgiver )
{
	uint64 giverGUID = questgiver->GetGUID();

	if( !bot->IsInMap( questgiver ) )
		aiRegistry->GetSocialManager()->TellMaster("hey you are turning in quests without me!");
	else
	{
		bot->SetSelectionGuid( ObjectGuid(giverGUID) );

		// auto complete every completed quest this NPC has
		bot->PrepareQuestMenu( giverGUID );
		QuestMenu& questMenu = bot->PlayerTalkClass->GetQuestMenu();
		for (uint32 iI = 0; iI < questMenu.MenuItemCount(); ++iI)
		{
			QuestMenuItem const& qItem = questMenu.GetItem(iI);
			uint32 questID = qItem.m_qId;
			Quest const* pQuest = sObjectMgr.GetQuestTemplate(questID);

			std::ostringstream out;
			std::string questTitle  = pQuest->GetTitle();
			QuestLocalization(questTitle, questID);

			QuestStatus status = bot->GetQuestStatus(questID);

			// if quest is complete, turn it in
			if (status == QUEST_STATUS_COMPLETE)
			{
				// if bot hasn't already turned quest in
				if (! bot->GetQuestRewardStatus(questID))
				{
					// auto reward quest if no choice in reward
					if (pQuest->GetRewChoiceItemsCount() == 0)
					{
						if (bot->CanRewardQuest(pQuest, false))
						{
							bot->RewardQuest(pQuest, 0, questgiver, false);
							out << "Quest complete: |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
						}
						else
						{
							out << "|cffff0000Unable to turn quest in:|r |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
						}
					}

					// auto reward quest if one item as reward
					else if (pQuest->GetRewChoiceItemsCount() == 1)
					{
						int rewardIdx = 0;
						ItemPrototype const *pRewardItem = sObjectMgr.GetItemPrototype(pQuest->RewChoiceItemId[rewardIdx]);
						std::string itemName = pRewardItem->Name1;
						aiRegistry->GetInventoryManager()->ItemLocalization(itemName, pRewardItem->ItemId);
						if (bot->CanRewardQuest(pQuest, rewardIdx, false))
						{
							bot->RewardQuest(pQuest, rewardIdx, questgiver, true);

							std::string itemName = pRewardItem->Name1;
							aiRegistry->GetInventoryManager()->ItemLocalization(itemName, pRewardItem->ItemId);

							out << "Quest complete: "
								<< " |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() 
								<< "|h[" << questTitle << "]|h|r reward: |cffffffff|Hitem:" 
								<< pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
						}
						else
						{
							out << "|cffff0000Unable to turn quest in:|r "
								<< "|cff808080|Hquest:" << questID << ':' 
								<< pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r"
								<< " reward: |cffffffff|Hitem:" 
								<< pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
						}
					}

					// else multiple rewards - let master pick
					else {
						out << "What reward should I take for |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() 
							<< "|h[" << questTitle << "]|h|r? ";
						for (uint8 i=0; i < pQuest->GetRewChoiceItemsCount(); ++i)
						{
							ItemPrototype const * const pRewardItem = sObjectMgr.GetItemPrototype(pQuest->RewChoiceItemId[i]);
							std::string itemName = pRewardItem->Name1;
							aiRegistry->GetInventoryManager()->ItemLocalization(itemName, pRewardItem->ItemId);
							out << "|cffffffff|Hitem:" << pRewardItem->ItemId << ":0:0:0:0:0:0:0" << "|h[" << itemName << "]|h|r";
						}
					}
				}
			}

			else if (status == QUEST_STATUS_INCOMPLETE) {
				out << "|cffff0000Quest incomplete:|r " 
					<< " |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
			}

			else if (status == QUEST_STATUS_AVAILABLE){
				out << "|cff00ff00Quest available:|r " 
					<< " |cff808080|Hquest:" << questID << ':' << pQuest->GetQuestLevel() << "|h[" << questTitle << "]|h|r";
			}

			if (! out.str().empty())
				aiRegistry->GetSocialManager()->TellMaster(out.str().c_str());
		}
	}
}

void AiQuestManager::AcceptQuest( Quest const *qInfo, Player *pGiver )
{
	if( !qInfo || !pGiver )
		return;

	uint32 quest = qInfo->GetQuestId();
	if( !bot->CanTakeQuest( qInfo, false ) )
	{
		// can't take quest
		bot->SetDividerGuid( ObjectGuid() );
        aiRegistry->GetSocialManager()->TellMaster("I can't take this quest");

		return;
	}

	if( !bot->GetDividerGuid().IsEmpty() )
	{
		// send msg to quest giving player
		pGiver->SendPushToPartyResponse( bot, QUEST_PARTY_MSG_ACCEPT_QUEST );
		bot->SetDividerGuid( ObjectGuid() );
	}

	if( bot->CanAddQuest( qInfo, false ) )
	{
		bot->AddQuest( qInfo, pGiver );

		if( bot->CanCompleteQuest( quest ) )
			bot->CompleteQuest( quest );

		// Runsttren: did not add typeid switch from WorldSession::HandleQuestgiverAcceptQuestOpcode!
		// I think it's not needed, cause typeid should be TYPEID_PLAYER - and this one is not handled
		// there and there is no default case also.

		if( qInfo->GetSrcSpell() > 0 )
			bot->CastSpell( bot, qInfo->GetSrcSpell(), true );

        aiRegistry->GetSocialManager()->TellMaster("Quest accepted");
	}
}

void AiQuestManager::HandleCommand(const string& text, Player& fromPlayer)
{
	if (text == "quests completed")
		ListQuests(QUEST_LIST_FILTER_COMPLETED);
	else if (text == "quests incompleted")
		ListQuests(QUEST_LIST_FILTER_INCOMPLETED);
	else if (text == "quests summary" || text == "qsummary")
		ListQuests(QUEST_LIST_FILTER_SUMMARY);
	else if (text == "quests")
		ListQuests(QUEST_LIST_FILTER_ALL);

	else if (text.size() > 5 && text.substr(0, 5) == "drop ")
	{
		DropQuest(text.substr(5).c_str());
	}

}

void AiQuestManager::HandleBotOutgoingPacket(const WorldPacket& packet)
{

}

void AiQuestManager::QueryQuestItem(uint32 itemId, const Quest *questTemplate, QuestStatusData *questStatus)
{
    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; i++)
    {
        if (questTemplate->ReqItemId[i] != itemId)
            continue;

        int required = questTemplate->ReqItemCount[i];
        int available = questStatus->m_itemcount[i];

        if (!required || available >= required)
            continue;

        ostringstream out;
        out << "Quest ";
        out << available;
        out << "/";
        out << required;
        aiRegistry->GetSocialManager()->TellMaster(out.str().c_str());
    }
}

void AiQuestManager::QueryQuestItem(uint32 itemId) 
{
    QuestStatusMap& questMap = bot->getQuestStatusMap();
    for (QuestStatusMap::iterator i = questMap.begin(); i != questMap.end(); i++)
    {
        const Quest *questTemplate = sObjectMgr.GetQuestTemplate( i->first );
        if( !questTemplate )
            continue;

        QuestStatusData *questStatus = &i->second;
        if( questStatus->m_status != QUEST_STATUS_INCOMPLETE )
            continue;

        QueryQuestItem(itemId, questTemplate, questStatus);
    }    
}


void AiQuestManager::QueryQuest(uint32 id) 
{
    for (uint16 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        if(id == bot->GetQuestSlotQuestId(slot))
        {
            ostringstream out;
            out << "Quest ";
            out << (bot->GetQuestStatus(id) == QUEST_STATUS_COMPLETE ? "completed" : "not completed");
            aiRegistry->GetSocialManager()->TellMaster(out.str().c_str());
            break;
        }
    }
}

void AiQuestManager::Query(const string& text)
{
    list<uint32> items;
    aiRegistry->GetInventoryManager()->extractItemIds(text, items);

    for (list<uint32>::iterator i = items.begin(); i != items.end(); i++)
        QueryQuestItem(*i);

 
    PlayerbotChatHandler ch(ai->GetMaster());
    uint32 questId = ch.extractQuestId(text.c_str());
    if (questId)
        QueryQuest(questId);
}
