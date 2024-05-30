#include<stdio.h>
#include<stdlib.h>
#include<time.h>

//파라미터
#define LEN_MIN 15  // 기차 길이
#define LEN_MAX 50
#define STM_MIN 0   // 마동석 체력
#define STM_MAX 5   
#define PROB_MIN 10   // 확률
#define PROB_MAX 90
#define AGGRO_MIN 0   // 어그로 범위
#define AGGRO_MAX 5

//마동석 이동 방향
#define MOVE_LEFT 1
#define MOVE_STAY 0

// 좀비의 공격 대상
#define ATK_NONE 0
#define ATK_CITIZEN 1
#define ATK_DONGSEOK 2

// 마동석 행동
#define ACTION_REST 0
#define ACTION_PROVOKE 1
#define ACTION_PULL 2

void intro();
void display_train();
int zombie_move();
int citizen_move();
void clear();
void over();
int madongseok_move();
int madongseok_action();
int action_pull();
int check_m_aggro();
int check_c_aggro();

int z_move, m_move, m_action, pull;
int c_move = 0;
int c_aggro = AGGRO_MIN;
int m_aggro = AGGRO_MIN;
int z_turn = 1;
int r, prob, m_stamina, len;
int z_stay, c_stay, m_stay;

int main(void)
{
    //intro
    intro();

    srand((unsigned int)time(NULL));
    r = rand() % 101;

    while (1)
    {
        printf("train length(15~50)>> ");
        scanf_s("%d", &len);
        if (LEN_MIN <= len && len <= LEN_MAX)
            break;
    }

    while (1)
    {
        printf("madongseok stamina(0~5)>> ");
        scanf_s("%d", &m_stamina);
        if (STM_MIN <= m_stamina && m_stamina <= STM_MAX)
            break;
    }

    while (1)
    {
        printf("percentile probability 'p'(10~90)>> ");
        scanf_s("%d", &prob);
        if (PROB_MIN <= prob && prob <= PROB_MAX)
            break;
    }

    z_stay = len - 3;
    c_stay = len - 6;
    m_stay = len - 2;
    //열차 초기 상태 출력
    display_train();

    // <이동> 페이즈
    while (1) {
        if (c_aggro < AGGRO_MIN)
            c_aggro = 1;
        else if (c_aggro > AGGRO_MAX)
            c_aggro = 5;
        else if (m_aggro < AGGRO_MIN)
            m_aggro = 1;
        else if (m_aggro > AGGRO_MAX)
            m_aggro = 5;

        //시민 이동
        citizen_move();
        //좀비 이동
        zombie_move();

        display_train();
        if (z_stay == c_stay + 1) {
            over();
            break;
        }
        else if (c_stay == 1) {
            clear();
            break;
        }
        //시민, 좀비, 마동석 상태 출력
        if (c_move == 1)
            printf("citizen: %d -> %d (aggro: %d)\n", c_stay + 1, c_stay, c_aggro);
        else
            printf("citizen: stay %d (aggro: %d)\n", c_stay, c_aggro);

        if (z_turn % 2 == 1)
            if (z_move == 1)
                printf("zombie: %d -> %d\n", z_stay + 1, z_stay);
            else
                printf("zombie: stay %d\n", z_stay);
        else
            printf("zombie: stay %d (cannot move)\n", z_stay);

        printf("\n");

        //마동석 이동
        madongseok_move();
        if (m_move == 0)
        {
            display_train();
            if (m_aggro > 0) {
                printf("madongseok: stay %d(aggro: %d -> %d, stamina: %d)\n", m_stay, m_aggro, m_aggro - 1, m_stamina);
                m_aggro--;
            }
            else {
                printf("madongseok: stay %d(aggro: %d, stamina: %d)\n", m_stay, m_aggro, m_stamina);
            }
        }
        else {
            if (m_aggro < AGGRO_MAX) {
                m_stay--;
                m_aggro++;
                display_train();
                printf("madongseok: %d -> %d(aggro: %d -> %d, stamina: %d)\n", m_stay + 1, m_stay, m_aggro - 1, m_aggro, m_stamina);
            }
            else {
                m_stay--;
                display_train();
                printf("madongseok: %d -> %d(aggro: %d, stamina: %d)\n", m_stay + 1, m_stay, m_aggro, m_stamina);
            }

        }

        //열차 상태 출력
        printf("\n");
        display_train();

        //<2> 행동 페이즈
        //(1) 시민행동
        printf("citizen does nothing.\n");

        //(2) 좀비 행동
        if (z_stay == c_stay + 1 && z_stay == m_stay - 1)
        {
            if (c_aggro > m_aggro) {
                printf("GAME OVER! citizen dead...\n");
                over();
                exit(0);
            }
            else {
                if (m_stamina > 1)
                {
                    printf("Zombie attacked madongseok (aggro: %d vs %d, madongseiok stamina: %d -> %d \n)", c_aggro, m_aggro, m_stamina, m_stamina - 1);
                    m_stamina -= 1;
                }
                else
                {
                    printf("madongseok dead...");
                    over();
                    exit(0);
                }
            }
        }
        else if (z_stay == c_stay + 1)
        {
            printf("GAME OVER! citizen dead...\n");
            over();
            exit(0);
        }
        else if (z_stay == m_stay - 1) {
            if (m_stamina > 1) {
                printf("Zombie attacked madongseok (aggro: %d vs %d, madongseiok stamina: %d -> %d \n)", c_aggro, m_aggro, m_stamina, m_stamina - 1);
                m_stamina -= 1;
            }
            else {
                printf("madongseok dead...");
                over();
                exit(0);
            }
        }
        else
            printf("zombie attacked nobody.\n");
        if (m_stamina == STM_MIN)
        {
            printf("GAME OVER! madongseok dead...\n");
            over();
            break;
        }
        //(3) 마동성 행동
        madongseok_action();

        if (m_action == 0)
        {
            printf("madongseok rests...\n");

            if(m_aggro==AGGRO_MIN)
                if(m_stamina==STM_MAX)
                    printf("madongseok: %d (aggro: %d, stamina: %d)\n", m_stay, m_aggro, m_stamina);
                else {
                    m_stamina++;
                    printf("madongseok: %d (aggro: %d, stamina: %d -> %d)\n", m_stay, m_aggro, m_stamina - 1, m_stamina);
                }
            else
                if (m_stamina == STM_MAX) {
                    m_aggro--;
                    printf("madongseok: %d (aggro: %d -> %d, stamina: %d)\n", m_stay, m_aggro + 1, m_aggro, m_stamina);
                }
                else {
                    m_aggro--;
                    m_stamina++;
                    printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n", m_stay, m_aggro + 1, m_aggro, m_stamina - 1, m_stamina);
                }

        }
        else if (m_action == 1)
        {
            printf("madongseok provoked zombie...\n");
            printf("madongseok: %d (aggro: %d -> %d, stamina: %d)\n", m_stay, m_aggro, AGGRO_MAX, m_stamina);
            m_aggro = AGGRO_MAX;
        }
        else if (m_action == 2)
        {
            action_pull();
            if (pull == 1) {
                printf("madongseok pulled zombie... Next turn, it can't move\n");
                z_turn++;
            }
            else
                printf("madongseok failed to pull zombie\n");
        }


        //마동석 행동결과

        //마동석 어그로,스태미나

        z_turn++;
     }
    return 0;
}

//인트로
void intro()
{
    printf("  ____     _     __  __  _____      ____  _____   _    ____  _____  \n");
    printf(" / ___|   / |   |   /  || ____|    / ___||_   _| / |  |  _  |_   _| \n");
    printf("| |  _   //_ |  | | /| ||  _|     (___     | |  / _ | | |_) ) | |   \n");
    printf("| |_| | / ___ | | |  | || |___     ___) )  | | / ___ ||  _ <  | |   \n");
    printf("  ____//_/   |_||_|  |_||_____|   |____/   |_|/ _/   _|_| |_| |_|   \n");
}
//열차상태 출력 함수
void display_train() {
    printf("\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < len; j++) {
            if (i == 0 || i == 2)
                printf("#");
            else {
                if (j == 0 || j == len - 1)
                    printf("#");
                else if (j == m_stay)
                    printf("M");
                else if (j == z_stay)
                    printf("Z");
                else if (j == c_stay)
                    printf("C");
                else
                    printf(" ");
            }
        }
        printf("\n");
    }
    printf("\n");
}
//좀비 이동 함수
int zombie_move() {
    if (z_turn % 2 == 1) {
        if (c_stay + 1 == z_stay && m_stay - 1 == z_stay)
            z_move = 0;
        else {
            if (c_aggro >= m_aggro)
                z_move = 1;
            else
                z_move = -1;
        }
    }
    else
        z_move = 0;
    z_stay -= z_move;

    return z_stay;
}
//시민 이동 함수
int citizen_move() {
    if (r >= prob) {
        c_move = 1;
        if(c_aggro<AGGRO_MAX)
             c_aggro += 1;
    }
    else {
        c_move = 0;
        if(c_aggro>AGGRO_MIN)
            c_aggro -= 1;
    }
    c_stay -= c_move;

    return c_stay;
}
//마동석 이동 함수
int madongseok_move() {
    if (z_stay + 1 == m_stay) {
        while (1) {
            printf("madongseok move(o:stay)>> ");
            scanf_s("%d", &m_move);
            if (m_move == 0)
                break;
        }
    }
    else
    {
        while (1) {
            printf("madongseok move(o:stay, 1:left)>> ");
            scanf_s("%d", &m_move);
            if (m_move == 1 || m_move == 0)
                break;
        }
    }
    return m_move;
}
//좀비 행동

//마동석 행동
int madongseok_action()
{
    if (m_stay == z_stay + 1)
    {
        printf("madongseio action(0.rest, 1.provoke, 2. pull)>> ");
        scanf_s("%d", &m_action);
    }
    else
    {
        printf("madongseio action(0.rest, 1.provoke)>> ");
        scanf_s("%d", &m_action);
    }
    return m_action;
}
//붙들기
int action_pull()
{
    m_aggro++;
    m_stamina--;
    if (r >= 100 - r)
        pull = 1;
    else
        pull = 0;

    return pull;
}
//아웃트로
void over()
{
    printf("  ____    _     __   __  _____      ___ __      ______  ____  \n");
    printf(" / ___|  / |   |    /  || ____|    / _ |  |   / | ____||  _ ) \n");
    printf("| |  _  / _ |  | | / | ||  _|     | | | || | / /|  _|  | |_) )\n");
    printf("| |_| |/ ___ | | |   | || |___    | |_| | | V / | |___ |  _ < \n");
    printf("  ____/_/   |_||_|   |_||_____|     ___/   |_/  |_____||_| |_|\n");

}
void clear()
{
    printf("  ____    _     __   __  _____     ____ _     _____    _    ____  \n");
    printf(" / ___|  / |   |    /  || ____|   / ___| |   | ____|  / |  |  _ ) \n");
    printf("| |  _  / _ |  | | / | ||  _|    | |   | |   |  _|   / _ | | |_) )\n");
    printf("| |_| |/ ___ | | |   | || |___   | |___| |___| |___ / ___ ||  _ < \n");
    printf("  ____/_/    _||_|   |_||_____|    ____|_____|_____/_/    _|_| |_| \n");

}