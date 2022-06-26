#include "Judge.h"

extern Player player;

bool hitCheckCircle(DirectX::XMFLOAT2 pos1, float r1, DirectX::XMFLOAT2 pos2, float r2)
{
    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    float r = r1 + r2;

    if (dx * dx + dy * dy <= r * r)
        return true;

    return false;
}

//--------------------------------------
//  OBJ2DìØémÇÃÇ†ÇΩÇËîªíË
//--------------------------------------
bool hitCheck(Enemy* enemy, Shot* shot)
{
    return hitCheckCircle(
        { enemy->getPos().x + 32, enemy->getPos().y + 32 }, 32,
        { shot->getPos().x + 8, shot->getPos().y + 8 }, 8
    );
}

//--------------------------------------
//  Ç†ÇΩÇËîªíË
//--------------------------------------
void judge(std::vector<Shot>& listOfShots, std::vector<Enemy>& listOfEnemies)
{
    // ÉvÉåÉCÉÑÅ[vsìGÇÃÇ†ÇΩÇËîªíË
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
