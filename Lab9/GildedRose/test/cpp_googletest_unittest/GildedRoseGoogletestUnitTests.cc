#include <gtest/gtest.h>
#include "GildedRose.h"

namespace {

    const std::string kBrie = "Aged Brie";
    const std::string kSulfuras = "Sulfuras, Hand of Ragnaros";
    const std::string kBackstage = "Backstage passes to a TAFKAL80ETC concert";
    const std::string kNormal = "Normal item";

    Item updateOnce(const std::string& name, int sellIn, int quality) {
        std::vector<Item> items;
        items.push_back(Item(name, sellIn, quality));
        GildedRose app(items);
        app.updateQuality();
        return app.items[0];
    }

}  

// Звичайні товари 

TEST(NormalItem, QualityDecreasesByOneAndSellInDecreases) {
    Item item = updateOnce(kNormal, 5, 10);
    EXPECT_EQ(9, item.quality);
    EXPECT_EQ(4, item.sellIn);
}

TEST(NormalItem, QualityDecreasesTwiceAsFastAfterSellDate) {
    EXPECT_EQ(8, updateOnce(kNormal, 0, 10).quality);
    EXPECT_EQ(8, updateOnce(kNormal, -1, 10).quality);
}

TEST(NormalItem, QualityIsNeverNegative) {
    EXPECT_EQ(0, updateOnce(kNormal, 5, 0).quality);
    EXPECT_EQ(0, updateOnce(kNormal, 0, 1).quality);
}

// Aged Brie 

TEST(AgedBrie, QualityIncreasesByOneBeforeSellDate) {
    EXPECT_EQ(11, updateOnce(kBrie, 5, 10).quality);
}

TEST(AgedBrie, QualityIncreasesByTwoAfterSellDate) {
    EXPECT_EQ(12, updateOnce(kBrie, 0, 10).quality);
}

TEST(AgedBrie, QualityNeverExceedsFifty) {
    EXPECT_EQ(50, updateOnce(kBrie, 5, 50).quality);
    EXPECT_EQ(50, updateOnce(kBrie, 0, 49).quality);
}

//Sulfuras

TEST(Sulfuras, QualityAndSellInNeverChange) {
    Item item = updateOnce(kSulfuras, 5, 80);
    EXPECT_EQ(80, item.quality);
    EXPECT_EQ(5, item.sellIn);
}

// Backstage passes

TEST(BackstagePasses, QualityIncreasesByOneWhenMoreThanTenDaysLeft) {
    EXPECT_EQ(11, updateOnce(kBackstage, 11, 10).quality);
}

TEST(BackstagePasses, QualityIncreasesByTwoWhenTenDaysOrLess) {
    EXPECT_EQ(12, updateOnce(kBackstage, 10, 10).quality);
    EXPECT_EQ(12, updateOnce(kBackstage, 6, 10).quality);
}

TEST(BackstagePasses, QualityIncreasesByThreeWhenFiveDaysOrLess) {
    EXPECT_EQ(13, updateOnce(kBackstage, 5, 10).quality);
}

TEST(BackstagePasses, QualityDropsToZeroAfterConcert) {
    EXPECT_EQ(0, updateOnce(kBackstage, 0, 10).quality);
}

TEST(BackstagePasses, QualityNeverExceedsFifty) {
    EXPECT_EQ(50, updateOnce(kBackstage, 10, 49).quality);
    EXPECT_EQ(50, updateOnce(kBackstage, 5, 49).quality);
}