#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

class cMarbleMatcher
{

    // input
    int theBucketCount;
    int theBucketCapacity;
    std::vector<int> theColorVector;

    // results
    std::vector<std::vector<int>> myBucketVector; // marbles in each bucket

    enum class eMatchState
    {
        waiting,
        done,
        impossible, // one or both are unavailable in marble supply
    };

    typedef std::map<std::pair<int, int>, eMatchState> matchMap_t;
    typedef matchMap_t::iterator matchMapIter_t;
    matchMap_t myMatchMap; // matches made, true or false

    int ConstructMatchMap()
    {
        for (int kc1 = 0; kc1 < theColorVector.size(); kc1++)
            for (int kc2 = kc1 + 1; kc2 < theColorVector.size(); kc2++)
                myMatchMap.insert(std::make_pair(
                    std::make_pair(kc1, kc2),
                    eMatchState::waiting));

        return myMatchMap.size();
    }

    void clearImpossible()
    {
        std::for_each(
            myMatchMap.begin(), myMatchMap.end(),
            [](matchMap_t::value_type &m)
            {
                if (m.second == eMatchState::impossible)
                    m.second = eMatchState::waiting;
            });
    }

    matchMapIter_t nextMatchWaiting()
    {
        return std::find_if(
            myMatchMap.begin(), myMatchMap.end(),
            [](matchMap_t::value_type &m) -> bool
            {
                return (m.second == eMatchState::waiting);
            });
    }
    int matchCount()
    {
        return std::count_if(
            myMatchMap.begin(), myMatchMap.end(),
            [](matchMap_t::value_type &m) -> bool
            {
                return (m.second == eMatchState::done);
            });
    }
    int nextBucketWithSpace(int req)
    {
        for (int nb = 0; nb < myBucketVector.size(); nb++)
            if (theBucketCapacity - myBucketVector[nb].size() >= req)
                return nb;
        return -1;
    }

    int BucketContainsOne(int nb, const std::pair<int, int> &cp)
    {
        for (int c : myBucketVector[nb])
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
        if (std::find(
                myBucketVector[nb].begin(), myBucketVector[nb].end(),
                color) != myBucketVector[nb].end())
            return;

        // add to bucket
        myBucketVector[nb].push_back(color);

        // remove from supply
        theColorVector[color]--;

        // mark all color pairs in bucket done
        for (int colorInBucket : myBucketVector[nb])
        {
            if (colorInBucket == color)
                continue;
            int c1 = color;
            int c2 = colorInBucket;
            if (c1 > c2)
            {
                c1 = colorInBucket;
                c2 = color;
            }
            myMatchMap.find(std::pair(c1, c2))->second = eMatchState::done;
        }
    }

    void addMarblePairs()
    {
        for (;;)
        {
            matchMapIter_t cp = nextMatchWaiting();
            if (cp == myMatchMap.end())
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
            addMarbleToBucket(nb, cp->first.first);
            addMarbleToBucket(nb, cp->first.second);

            // mark color pair matched
            cp->second = eMatchState::done;
        }
    }

    void addMarbleSingles()
    {
        for (;;)
        {
            matchMapIter_t cp = nextMatchWaiting();
            if (cp == myMatchMap.end())
                break;

            // std::cout << "trying for match " << cp->first.first << "," << cp->first.second << "\n";

            for (int nb = 0; nb < myBucketVector.size(); nb++)
            {
                if (theBucketCapacity - myBucketVector[nb].size() > 0)
                {
                    int has = BucketContainsOne(nb, cp->first);
                    if (has != -1)
                    {
                        // add missing marble of pair, if available
                        if (has == cp->first.first)
                        {
                            if (theColorVector[cp->first.second])
                            {
                                addMarbleToBucket(nb, cp->first.second);
                                cp->second = eMatchState::done;
                                break;
                            }
                        }
                        if (has == cp->first.second)
                        {
                            if (theColorVector[cp->first.first])
                            {
                                addMarbleToBucket(nb, cp->first.first);
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

    void set(
        int bucketCount,
        int bucketcapacity,
        const std::vector<int>& vColor    )
    {
        theBucketCount = bucketCount;
        theBucketCapacity = bucketcapacity;
        theColorVector = vColor;
    }

    void solve()
    {
        // construct result data structure
        myBucketVector.resize(theBucketCount);
        ConstructMatchMap();

        // add marbles in pairs
        addMarblePairs();

        // buckets with even capacity, sufficient for all colors, will now be full
        if (theBucketCapacity % 2 == 0)
            return;

        clearImpossible();

        // add marbles one by one
        addMarbleSingles();
    }

    void display()
    {
        std::cout << "\nShared Colors: " << matchCount()
                  << "\nColors in each bucket:\n";
        for (auto &vc : myBucketVector)
        {
            for (int c : vc)
                std::cout << c << ", ";
            std::cout << "\n";
        }
    }

    static bool tests()
    {
        bool pass = true;
        cMarbleMatcher matcher;

        matcher.set(8,5,{4,4,4,4,4,4,4,4,4,4,});
        matcher.solve();
        if( matcher.matchCount() != 42 )
        {
            std::cout << "8 Bucket test failed\n";
            pass = false;
        }

        return pass;
    }
};

main()
{
    if( ! cMarbleMatcher::tests() )
        return 1;

    cMarbleMatcher theMarbleMatcher;

    theMarbleMatcher.getInput();

    theMarbleMatcher.solve();

    theMarbleMatcher.display();

    return 0;
}
