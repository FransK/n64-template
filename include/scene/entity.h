#pragma once

namespace Fishing
{
    static int NextEntityId = 1;

    static int GetNextEntityId()
    {
        return NextEntityId++;
    }
}