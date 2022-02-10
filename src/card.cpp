#include "freesolitaire/stdafx.h"
#include "freesolitaire/card.h"
#include "freesolitaire/atlas.h"

using FreeSolitaire::Card;
using FreeSolitaire::Sprite;
using FreeSolitaire::SpriteAtlasTexture;

extern SpriteAtlasTexture* pokers;

void Card::getStrName(bool isUnknown,
    Suit suit,
    signed char number,
    char* buf) {
    if (isUnknown) {
        strcpy_s(buf, 3, "00");
        return;
    }
    if (number >= 10) {
        memset(buf, 0, 4);
        switch (suit) {
        case Suit::DIAMONDS:
            buf[0] = 'D';
            break;
        case Suit::CLUBS:
            buf[0] = 'C';
            break;
        case Suit::HEARTS:
            buf[0] = 'H';
            break;
        case Suit::SPADES:
            buf[0] = 'S';
            break;
        }
        buf[1] = '1';
        buf[2] = number - 10 + '0';
        return;
    }
    memset(buf, 0, 3);
    switch (suit) {
    case Suit::DIAMONDS:
        buf[0] = 'D';
        break;
    case Suit::CLUBS:
        buf[0] = 'C';
        break;
    case Suit::HEARTS:
        buf[0] = 'H';
        break;
    case Suit::SPADES:
        buf[0] = 'S';
        break;
    }
    buf[1] = number + '0';
}
void Card::getStrName(char* __buf) {
    getStrName(isUnknown, suit, number, __buf);
}
void Card::render() {
    if (isUnknown) {
        Sprite& sprite = pokers->getSprite("00");
        glBegin(GL_QUADS);
        glTexCoord2f(sprite.u0, sprite.v0);
        glVertex2i(0, 0);
        glTexCoord2f(sprite.u0, sprite.v1);
        glVertex2i(0, sprite.h);
        glTexCoord2f(sprite.u1, sprite.v1);
        glVertex2i(sprite.w, sprite.h);
        glTexCoord2f(sprite.u1, sprite.v0);
        glVertex2i(sprite.w, 0);
        glEnd();
    }
}
