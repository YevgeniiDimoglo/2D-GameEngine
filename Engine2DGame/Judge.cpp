#include "Judge.h"

bool hitCheckCircle(DirectX::XMFLOAT2 pos1, float r1, DirectX::XMFLOAT2 pos2, float r2)
{
    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    float r = r1 + r2;

    if (dx * dx + dy * dy <= r * r)
        return true;

    return false;
}


bool hitCheck(Enemy* enemy, Shot* shot)
{   
    if (enemy->getAct() == 0) return 0;

    return hitCheckCircle(
        { enemy->getPos().x + 32, enemy->getPos().y + 32 }, 32,
        { shot->getPos().x + 8, shot->getPos().y + 8 }, 8
    );
}

bool hitCheckPlayer(Player& player, Shot* shot)
{
    if (player.getAct() == 0) return 0;

    return hitCheckCircle(
        { player.getPos().x + 60, player.getPos().y + 60 }, 40,
        { shot->getPos().x + 8, shot->getPos().y + 8 }, 8
    );
}


void judge(std::vector<Shot>& listOfShots, std::vector<Enemy>& listOfEnemies)
{
    for (int i = 0; i < 512; ++i)
    {
        if (listOfShots[i].getAct() != 10)
        {
            for (int j = 0; j < 100; ++j)
            {
                if (hitCheck(&listOfEnemies[j], &listOfShots[i]))
                {
                    listOfEnemies[j].setHP(listOfEnemies[j].getHP() - 1);
                    listOfShots[i].setAct(10);
                }
            }

        }
    }
}

void judgePlayer(Player& player, std::vector<Shot>& listOfShots)
{
    for (int i = 0; i < 512; ++i)
    {
        if (listOfShots[i].getAct() != 10)
        {
            if (hitCheckPlayer(player, &listOfShots[i]))
            {
                player.setHP(player.getHP() - 1);
                listOfShots[i].setAct(10);
            }
        }
    }
}

//void judge2()
//{
//    for (int i = 0; i < SHOT_MAX; ++i)
//    {
//        if (!shot[i].moveAlg)              continue;
//        if (shot[i].area != player.area)   continue;
//
//        if (hitCheck(&player, &shot[i]))
//        {
//            --player.hp;
//            shot[i].moveAlg = NULL;
//
//            if (player.hp == 0)
//            {
//                game_reset();
//            }
//        }
//    }
//}
