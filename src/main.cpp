#include <string>
#include <iostream>
#include <vector>
#include <map>

class cMarbleMatcher {

// input
int theBucketCount;
int theBucketCapacity;
std::vector<int> theColorVector;

// results
std::vector<std::vector<int>> theBucketVector; // marbles in each bucket

enum class eMatchState
{
    waiting,
    done,
    impossible, // one or both are unavailable in marble supply
};

typedef std::map<std::pair<int, int>, eMatchState> matchMap_t;
typedef matchMap_t::iterator matchMapIter_t;
matchMap_t theMatchMap; // matches made, true or false

int ConstructMatchMap()
{
    for (int kc1 = 0; kc1 < theColorVector.size(); kc1++)
        for (int kc2 = kc1 + 1; kc2 < theColorVector.size(); kc2++)
            theMatchMap.insert(std::make_pair(
                std::make_pair(kc1, kc2),
                eMatchState::waiting));

    return theMatchMap.size();
}

void ClearImpossible()
{
    matchMapIter_t i;
    for (
        i = theMatchMap.begin();
        i != theMatchMap.end();
        i++)
    {
        if (i->second == eMatchState::impossible)
        {
            i->second = eMatchState::waiting;
        }
    }
}

matchMapIter_t nextMatchWaiting()
{
    matchMapIter_t i;
    for (
        i = theMatchMap.begin();
        i != theMatchMap.end();
        i++)
    {
        if (i->second == eMatchState::waiting)
        {
            return i;
        }
    }
    return i;
}
int MatchCount()
{
    int count = 0;
    matchMapIter_t i;
    for (
        i = theMatchMap.begin();
        i != theMatchMap.end();
        i++)
    {
        if (i->second == eMatchState::done)
            count++;
    }
    return count;
}
int nextBucketWithSpace(int req)
{
    for (int nb = 0; nb < theBucketVector.size(); nb++)
        if (theBucketCapacity - theBucketVector[nb].size() >= req)
            return nb;
    return -1;
}

int BucketContainsOne(int nb, const std::pair<int, int> &cp)
{
    for (int c : theBucketVector[nb])
    {
        if (c == cp.first)
            return cp.first;
        if (c == cp.second)
            return cp.second;
    }
    return -1;
}
/// @brief Add marble to bucket
/// @param nb bucket to be added to
/// @param color color of marble added
///
/// NOP if color is already in bucket

void addMarbleToBucket(
    int nb,
    int color)
{
    // check for color already present TID1
    for( int colorInBucket : theBucketVector[nb] )
            if( colorInBucket == color )
                return;

    // add to bucket
    theBucketVector[nb].push_back(color);

    // remove from supply
    theColorVector[color]--;

}



void AddMarblePairs()
{
    for (;;)
    {
        matchMapIter_t cp = nextMatchWaiting();
        if (cp == theMatchMap.end())
            break;
        int nb = nextBucketWithSpace(2);
        if (nb == -1)
            break;

        // check both marbles are available
        if ((!theColorVector[cp->first.first]) ||
            (!theColorVector[cp->first.second]))
        {
            cp->second = eMatchState::impossible;
            continue;
        }

        // std::cout << "Adding colors " << cp->first.first << "," << cp->first.second
        //           << " to bucket " << nb << "\n";

        // add pair of marbles to bucket
        addMarbleToBucket(nb,cp->first.first);
        addMarbleToBucket(nb,cp->first.second);

        // mark color pair matched
        cp->second = eMatchState::done;
    }
}

void AddMarbleSingles()
{
    for (;;)
    {
        matchMapIter_t cp = nextMatchWaiting();
        if (cp == theMatchMap.end())
            break;

        // std::cout << "trying for match " << cp->first.first << "," << cp->first.second << "\n";

        for (int nb = 0; nb < theBucketVector.size(); nb++)
        {
            if (theBucketCapacity - theBucketVector[nb].size() > 0)
            {
                int has = BucketContainsOne(nb, cp->first);
                if (has != -1)
                {
                    // add missing marble of pair, if available
                    if (has == cp->first.first)
                    {
                        if (theColorVector[cp->first.second])
                        {
                            addMarbleToBucket(nb,cp->first.second);
                            cp->second = eMatchState::done;
                            break;
                        }
                    }
                    if (has == cp->first.second)
                    {
                        if (theColorVector[cp->first.first])
                        {
                            addMarbleToBucket(nb,cp->first.first);
                            cp->second = eMatchState::done;
                            break;
                        }
                    }
                    if (cp->second != eMatchState::done)
                        cp->second = eMatchState::impossible;
                }
            }
        }
    }
}

public:

void getInput()
{
    std::cout << "Bucket Count: ";
    std::cin >> theBucketCount;

    std::cout << "Bucket Capacity: ";
    std::cin >> theBucketCapacity;

    std::cout << "Unique Color Count: ";
    int colorCount;
    std::cin >> colorCount;

    for (int kc = 0; kc < colorCount; kc++)
    {
        std::cout << "Color " << kc << " Count: ";
        int c;
        std::cin >> c;
        theColorVector.push_back(c);
    }
}

void solve()
{
    // construct result data structure
    theBucketVector.resize(theBucketCount);
    ConstructMatchMap();

    // add marbles in pairs
    AddMarblePairs();

    // buckets with even capacity, sufficient for all colors, will now be full
    if (theBucketCapacity % 2 == 0)
        return;

    ClearImpossible();

    // add marbles one by one
    AddMarbleSingles();
}

void display()
{
    std::cout << "\nShared Colors: " << MatchCount()
              << "\nColors in each bucket:\n";
    for (auto &vc : theBucketVector)
    {
        for (int c : vc)
            std::cout << c << ", ";
        std::cout << "\n";
    }
}

};

main()
{
    cMarbleMatcher theMarbleMatcher;

    theMarbleMatcher.getInput();

    theMarbleMatcher.solve();

    theMarbleMatcher.display();

    return 0;
}
