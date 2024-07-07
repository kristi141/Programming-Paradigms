/*RU
Напишите программу, которая будет выдавать правильный ответ в игре SET
В игре SET участвует 81 карта, каждая с уникальной картинкой
Каждая карта имеет 4 признака, перечисленных ниже:
Количество: на каждой карте есть один, два или три символа.
Тип символов:  овалы, ромбы или волны.
Цвет: символы могут быть красными, зелеными или фиолетовыми.
Заполнение: символы могут быть пустыми, заштрихованными или закрашенными.
Цель игры SET: Среди 12 карт, разложенных на столе, нужно найти SET (набор), 
состоящий ровно из 3-х карт, у которых каждый из признаков либо полностью 
совпадает, либо полностью различается на всех 3-х картах. Все признаки должны 
полностью подчиняться этому правилу.
Например, если количество только символов на всех 3 картах одинаково, то цвет, 
тип и заполнение должны у всех различаться 

Входные данные:
Массив структур [CARD,...,CARD]

Выходные данные:
Вывести индексы элементов массива во всех найденных SET’ах
1: 2 3 5 (SET первый, карты с индексами 2, 3, 5 во входном массиве)
2: 6 9 11
3: 1 2 3

ENG
Write a program that will give the correct answer in the SET game
The SET game features 81 cards, each with a unique picture.
Each card has 4 characteristics listed below:
Quantity: Each card has one, two or three symbols.
Type of symbols: ovals, diamonds or waves.
Color: Symbols can be red, green or purple.
Fill: Characters can be blank, shaded, or solid.
Purpose of the game SET: Among the 12 cards laid out on the table, you need 
to find the SET (set), consisting of exactly 3 cards, each of which has 
either completely matches or is completely different on all 3 maps. All signs must
completely obey this rule.
For example, if the number of only symbols on all 3 cards is the same, then 
the color, type and filling should be different for everyone

Input data:
Array of structures [CARD,...,CARD]

Output:
Print the indices of array elements in all found SETs
1: 2 3 5 (SET first, cards with indices 2, 3, 5 in input array)
2: 6 9 11
3: 1 2 3                        */

#include <iostream>
#include <vector>
using namespace std;

struct Card {
    int Quantity;
    int Type;
    int Color;
    int Filling;

    Card(int quantity, int type, int color, int filling) : Quantity(quantity), Type(type), Color(color), Filling(filling) {}
};

vector<Card> fillCards(vector<Card> cards, int i=0) {
    if (!(i == 12)) {
        vector<Card> cardsCopy = cards;
        cin >> cardsCopy[i].Quantity >> cardsCopy[i].Type >> 
               cardsCopy[i].Color >> cardsCopy[i].Filling;
        return fillCards(cardsCopy, i + 1)
    }
    return cards;
}

bool checkSet(const Card firstCard, const Card secondCard, const Card thirdCard) {
    return ((firstCard.Quantity == secondCard.Quantity && secondCard.Quantity == thirdCard.Quantity) || 
        (firstCard.Quantity != secondCard.Quantity && secondCard.Quantity != thirdCard.Quantity)) &&
        ((firstCard.Type == secondCard.Type && secondCard.Type == thirdCard.Type) ||
         (firstCard.Type != secondCard.Type && secondCard.Type != thirdCard.Type)) &&
        ((firstCard.Color == secondCard.Color && secondCard.Color == thirdCard.Color) ||
         (firstCard.Color != secondCard.Color && secondCard.Color != thirdCard.Color)) &&
        ((firstCard.Filling == secondCard.Filling && secondCard.Filling == thirdCard.Filling) ||
         (firstCard.Filling != secondCard.Filling && secondCard.Filling != thirdCard.Filling));
}

bool checkCards(int i, int j, int k, vector<Card> cards) {
    return !((cards[i].Quantity == cards[j].Quantity && cards[j].Quantity == cards[k].Quantity) &&
        (cards[i].Type == cards[j].Type && cards[j].Type == cards[k].Type) &&
        (cards[i].Color == cards[j].Color && cards[j].Color == cards[k].Color) &&
        (cards[i].Filling == cards[j].Filling && cards[j].Filling == cards[k].Filling) &&
        checkSet(cards[i], cards[j], cards[k]))
}

vector<vector<int>> findSets(vector<Card> cards,
    vector<vector<int>> answer,
    int i = 0, int j = 0, int k = 0,
    int currentAnswer = 0)
{
    if (checkCards(i, j, k, cards) && checkSet(cards[i], cards[j], cards[k])) {
        vector<vector<int>> answerCopy = answer;
        answerCopy[currentAnswer] = vector<int>{i, j, k};
        return (i == 12) ? (j == 12) ? (k == 12) ?
            answerCopy :
            findSets(cards, answerCopy, i, j, k + 1, currentAnswer + 1) :
            findSets(cards, answerCopy, i, j + 1, k, currentAnswer + 1) :
            findSets(cards, answerCopy, i + 1, j, k, currentAnswer + 1);
    }
    return (i == 12) ? (j == 12) ? (k == 12) ?
        answer:
        findSets(cards, answer, i, j, k + 1, currentAnswer) :
        findSets(cards, answer, i, j + 1, k, currentAnswer) :
        findSets(cards, answer, i + 1, j, k, currentAnswer);
}

void print(const vector<vector<int>> sets, int index) {
    cout << sets[index][0] << sets[index][1], sets[index][2];
    cout << endl;
}

void printSets(const vector<vector<int>> sets, int index = 0) {
    return (index == 220 ? void() :
         print(sets, index),
         printSets(sets, index + 1));
}

int main() {
    printSets(findSets(fillCards(vector<Card>(12)), vector<vector<int>>(220, vector<int>(3))));
    return 0;
}