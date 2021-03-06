/*
 * Copyright (C) 2011-2013 /dev/rsa for MangosR2 <http://github.com/MangosR2>
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _MAP_UPDATER_H_INCLUDED
#define _MAP_UPDATER_H_INCLUDED

#include "ObjectUpdateTaskBase.h"
#include "Common.h"

class Map;

struct MapBrokenData
{
    explicit MapBrokenData()
    {
        Reset();
    }

    void Reset()
    {
        count = 1;
        lastErrorTime = WorldTimer::getMSTime();
    };

    void IncreaseCount() { ++count; lastErrorTime = WorldTimer::getMSTime();};
    uint32 count;
    uint32 lastErrorTime;
};

typedef std::map<Map*,MapBrokenData> MapBrokenDataMap;

class MapUpdater : public ObjectUpdateTaskBase<class Map>
{
    public:

        MapUpdater() : ObjectUpdateTaskBase<class Map>()
        {}

        virtual ~MapUpdater() {};

        Map* GetMapByThreadId(ACE_thread_t const threadId);
        void FreezeDetect();

        void MapBrokenEvent(Map* map);
        MapBrokenData const* GetMapBrokenData(Map* map);

    private:
        MapBrokenDataMap   m_brokendata;
};

#endif //_MAP_UPDATER_H_INCLUDED
