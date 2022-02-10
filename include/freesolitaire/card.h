#pragma once

namespace FreeSolitaire {
    enum class Suit : signed char {
        // 方块
        DIAMONDS = 1,
        // 梅花
        CLUBS,
        // 红心
        HEARTS,
        // 黑桃
        SPADES
    };

    struct Card {
        bool isUnknown;
        Suit suit;
        signed char number;

        static void getStrName(bool isUnknown,
            Suit suit,
            signed char number,
            char* buf);
        void getStrName(char* __buf);
        void render();
    };
}
