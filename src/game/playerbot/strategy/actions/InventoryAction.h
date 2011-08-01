#pragma once

#include "../Action.h"
#include "../ItemVisitors.h"

namespace ai
{


    class InventoryAction : public Action {
    public:
        InventoryAction(PlayerbotAI* ai, string name) : Action(ai, name) {}

    protected:
        void IterateItems(IterateItemsVisitor* visitor, IterateItemsMask mask = ITERATE_ITEMS_IN_BAGS);
        void TellItem(ItemPrototype const * proto, int count);
        list<Item*> parseItems(string text);

    private:
        void IterateItemsInBags(IterateItemsVisitor* visitor);
        void IterateItemsInEquip(IterateItemsVisitor* visitor);
    };
}
