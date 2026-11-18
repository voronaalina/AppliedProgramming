#include "GildedRose.h"
#include "UpdateItem.h"
#include "IcecreamUpdater.h"
#include <map>

using namespace std;

namespace {

    const ItemUpdater& selectUpdater(const string& name) {
        static const AgedBrieUpdater agedBrie{};
        static const SulfurasUpdater sulfuras{};
        static const BackstagePassUpdater backstagePass{};
        static const NormalItemUpdater normal{};
        static const IceCreamUpdater iceCream{ Temperature::Normal };

        static const map<string, const ItemUpdater*> updaters = {
            {kAgedBrie, &agedBrie},
            {kSulfuras, &sulfuras},
            {kBackstagePasses, &backstagePass},
            {kIceCream, &iceCream},
        };

        auto found = updaters.find(name);
        if (found != updaters.end()) {
            return *found->second;
        }
        return normal;  
    }

} 

GildedRose::GildedRose(vector<Item>& items) : items(items)
{
}

void GildedRose::updateQuality()
{
    for (auto& item : items)
    {
        selectUpdater(item.name).update(item);
    }
}
