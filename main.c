#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <conio.h>
#include <string.h>
#include <stdbool.h>
#ifdef _WIN32
#include <io.h>
#include <windows.h>
#define ACCESS _access
#else
#include <unistd.h>
#define ACCESS access
#endif


enum EvadeActionStatus
{
    Succeed,
    Failed,
    Grazed
};
enum AchievementsList
{
    Overkill, //Deal Over 60 Damage in a single attack
    Perfectionist, //In the rapier attack, wait a long time before to use the attack
    Perfect_Timing, //Get a critical hit on the rapier attack
    Venture_Forth, //Encounter 10 or more enemies
    Trash, //Lose in less than 3 turns
    Upgraded, //Purchase your first upgrade in the shop
    Overprepared, //Acquire every upgrade item in the shop.
    BigBacked //Spend the entire duration of your turn eating food
};
enum AchievementsList Achievenment;
enum EvadeActionStatus EvadeAction;

//Save Game and Play later
typedef struct
{

float playerHealth;
float maxHealth;

//Shop Variables
float coins; //Change back to 0
bool armorLevel1,armorLevel2;
bool sharpness1,sharpness2;
bool healthBoost1,healthBoost2;
bool hasRapier; //Change Bck to false
bool isRapierCooldown;
float armorResistance;



int foodAmount;
int bigbacktracker;


float bladeShapnessMulti;
bool _overkill, _perfectionist, _perfectTiming, _ventureForth, _trash, _upgraded, _overprepared, _bigbacked;

char playerName[80];
float rerollNumber;
int achievementsCounter;

} GameState;


int turns;
int a,b,c,d;
char s;
float initEnemyHealth;
bool rerolled = false;

//pointers



//Prototypes
void ShopText(GameState* state);
float Attack(int a, float subjectHealth, int attackProfile, float DamageReductionMultiplier, float EnemyResistanceMultiplier, GameState* state);
enum EvadeActionStatus DodgeOrParry(int b);
void WinGame(GameState* state);
void LoseGame(GameState* state);
float RandomizeEnemyHealth();
void AchievementGiver(enum AchievementsList Achievenment,GameState* state);
float clamp(float value, float min, float max);

//GameSaveStateFunctions
void saveGameState(const char* filename, const GameState* state);
int loadGameState(const char* filename, GameState* state);
void initializeState(GameState* state);
bool justLoadedIn;
//Animations
void ClearScreen();
void delay(int miliseconds);


int saveFileExists(const char* filename) {
    // Check if the file exists
    if (ACCESS(filename, 0) == 0) {
        return 1; // File exists
    } else {
        return 0; // File does not exist
    }
}

int main()
{
    GameState state;
    const char* filename = "VentureForthProgress.dat";


    float *amr = &state.armorResistance;
    float *storedCoins = &state.coins;
    int *storedFood = &state.foodAmount;
    float *sharpPointer = &state.bladeShapnessMulti;


    float enemyHealth;
    printf("Welcome to \033[93m**Venture Forth RPG**\033[0m, Press Enter to Continue\n");
    getchar();


    int gameSelect;
    int choice;
    int choiceConfirm;
    for(gameSelect=1;gameSelect>0;gameSelect++)
    {
        bool saveFound;
        ClearScreen();
        printf("Choose an option below to get started\n");
        printf("--------------------------------------\n");
        printf("\t(0) -- Start New Game\n");
        printf("(0) -- Starts a new game with completely empty stats\n");
        if(loadGameState(filename, &state))
        {
            printf("\t(1) -- Access Save File\n");
            printf("(*) - Player Name: %s\n    - Coins: %.2f\n    - Health: %.2f\n    - Achievements Earned: %d\n    - Level: %d\n",state.playerName,state.coins,state.playerHealth,state.achievementsCounter,(int)state.rerollNumber);
            saveFound = true;
        }
        else
        {
            printf("\t\033[90m(*) -- No Save File Found\033[0m\n");
            saveFound = false;
        }


        scanf("%d",&choice);
        while ((getchar()) != '\n');


        if(choice == 0)
        {
            printf("Are you sure you want to start a new game? Note that this will overwrite\nany previous save file. Type 1 to confirm.\n");
            scanf("%d",&choiceConfirm);
            while ((getchar()) != '\n');

            if(choiceConfirm == 1)
            {
                initializeState(&state);

                printf("\t\033[90m-- New Save Started --\033[0m\n");
                printf("Please Enter your name:\n");
                scanf("%s", state.playerName);
                while ((getchar()) != '\n');

                printf("Okay. Valiant Warrior \033[32m%s\033[0m, Press Enter to Venture Onward!\n", state.playerName);
                getchar();
                saveGameState(filename, &state);


                state.playerHealth = 20;
                state.maxHealth = 20;
                enemyHealth = 30;
                initEnemyHealth = enemyHealth;
                printf("Player Health is %.2f. Enemy Health is %.2f\n", state.playerHealth, enemyHealth);
                getchar();
                break;
            }
            else
            {
                printf("Invalid Option. Press Enter to return to main menu\n");
                getchar();
            }
        }
        else if(choice == 1 && saveFound)
        {
            printf("Load Player \033[92m%s\033[0m Save File? Enter 1 to confirm.\n",state.playerName);
            scanf("%d",&choiceConfirm);
            while ((getchar()) != '\n');
            if(choiceConfirm == 1)
            {
                printf("Awesome! Game State Successfully loaded!\n");
                justLoadedIn = true;
                enemyHealth = 0;
                break;
            }
            else
            {
                printf("Invalid option. Press Enter to return to main menu\n");
                getchar();
            }

        }
        else
        {
            printf("Invalid option. Press enter to continue\n");
            getchar();
        }
    }


    int tries;


    const char* framesAttack[] = {
    "[|========={==}==========]",
    "[=|========{==}==========]",
    "[==|======={==}==========]",
    "[===|======{==}==========]",
    "[====|====={==}==========]",
    "[=====|===={==}==========]",
    "[======|==={==}==========]",
    "[=======|=={==}==========]",
    "[========|={==}==========]",
    "[=========|{==}==========]",
    "[=========={\\=}==========]",
    "[=========={=\\}==========]",
    "[=========={==}|=========]",
    "[=========={==}=|========]",
    "[=========={==}==|=======]",
    "[=========={==}===|======]",
    "[=========={==}====|=====]",
    "[=========={==}=====|====]",
    "[=========={==}======|===]",
    "[=========={==}=======|==]",
    "[=========={==}========|=]",
    "[=========={==}=========|]",
    };
    int numFrames = sizeof(framesAttack) / sizeof(framesAttack[0]);

    double t = 0.0;
    double speed = 0.3;
    int frameIndex;

    int isKeyPressed() {
    #ifdef _WIN32
    return _kbhit(); // Returns 1 if a key is pressed, 0 otherwise
    #else
    struct termios oldt, newt;
    int ch;
    int oldf;

    // Get the current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Disable canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Check if a key is pressed
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    // Restore the terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin); // Push the character back to stdin
        return 1; // Key was pressed
    }

    return 0; // No key was pressed
    #endif
}

    printf("Press enter to continue...\n");
    getchar();

    for(turns=1; turns > 0; turns++)
    {
        if(rerolled)
        {
            if (state.rerollNumber == 10 && !state._ventureForth)
            {
                AchievementGiver(Venture_Forth,&state);
            }
            state.playerHealth = (state.playerHealth <= 0 ? 20 : ((state.playerHealth < 8 ? state.playerHealth+4 : state.playerHealth)))+(state.healthBoost1 ? 5 : 0)+(state.healthBoost1 ? 10 : 0);
            state.maxHealth = 20 + (state.healthBoost1 ? 25 : 0) + (state.healthBoost2 ? 35 : 0);

            *sharpPointer = 1 * (state.sharpness1 ? 1.2 : 1) * (state.sharpness2 ? 1.2 : 1);
            float scalingFactor = 1 + sqrt(state.rerollNumber) / 5;
            int times;
            for(times=0;times<floor(state.rerollNumber/8);times++)
            {
                scalingFactor*=1.08;
            }
            enemyHealth = RandomizeEnemyHealth()*scalingFactor;
            initEnemyHealth = enemyHealth;
            if(state.playerHealth > state.maxHealth) state.playerHealth = state.maxHealth;
            //Update ArmorValues
            *amr = 1 * (state.armorLevel1 ? 0.8f : 1) * (state.armorLevel2 ? 0.5f : 1);
            printf("Player Health is %.2f/%.0f. The enemy's health is %.2f\n", state.playerHealth, state.maxHealth, enemyHealth);
            rerolled = false;
            state.rerollNumber += 1;
            turns = 1;
            state.bigbacktracker = 0;
        }
        else if(enemyHealth <= 0 || state.playerHealth <= 0)
        {
            if(!justLoadedIn)
            {
                if(enemyHealth <=0) WinGame(&state);
                else if(state.playerHealth <=0)
                {
                    LoseGame(&state);
                    float coinLossPercentage = 0.35f; // Base percentage of coins lost (20%)
                    float enemyHealthFactor = initEnemyHealth / 100.0f; // Scale by enemy health
                    float randomFactor = 0.8f + (rand() % 41) / 100.0f; // Random factor between 0.8 and 1.2

                    int coinsLost = (int)(*storedCoins * coinLossPercentage * enemyHealthFactor * randomFactor);

                    // Ensure coinsLost is within a reasonable range
                    coinsLost = fmax(10, fmin(coinsLost, *storedCoins)); // Lose at least 10 coins, but not more than the player has


                    if(*storedCoins - coinsLost < 0)
                    {
                        coinsLost = *storedCoins;
                    }
                    *storedCoins -= coinsLost;

                    state.rerollNumber = floor(fmax(state.rerollNumber / 4, 0));
                    printf("\033[31mYou lost some coins!\033[0m\n");
                    if(7-turns>0 && !state._trash)
                    {
                        AchievementGiver(Trash,&state);
                    }
                }
                printf("Press enter to continue...\n");
                getchar();
                int earnedCoins = 0;
                double baseReward = 3*log(initEnemyHealth*1.6 + 1);

                double rewardDiminish = 1.35;
                double scaledReward = baseReward / rewardDiminish;

                double randomFactor = 0.8 + (rand() % 51) / 100;
                double finalReward = scaledReward*randomFactor;

                if(enemyHealth <= 0) earnedCoins = (int)finalReward;
                else if(state.playerHealth <= 0) earnedCoins = ((rand() % ((int)(initEnemyHealth - ((initEnemyHealth*.2)+1)) + (int)(initEnemyHealth*.2)))*0.21);

                float overflowBonus = (int)((enemyHealth <= 0 ? 1.1*sqrt(enemyHealth*-1.36) : 0));
                state.coins += (earnedCoins + overflowBonus);

                // Ensure coins don't go negative after earning

                printf("You Earned %d coins! Health OverFlow Bonus - %.2f. You now have %.2f\n", earnedCoins, overflowBonus, state.coins);
                printf("Press enter to continue...\n");
                getchar();
            }
            if (justLoadedIn) justLoadedIn = false;
            int j;
            for(j=1;j>0;j++)
            {
                ClearScreen();
                printf("Where to \033[92m%s?\033[0m\n",state.playerName);
                printf("\t(0) -- Venture Forward\n");
                printf("(0) -- Keeps going on to fight different enemies. Enemies get harder the longer you stay alive!!\n");
                printf("\t(1) -- Open Shop\n");
                printf("(1) -- Opens the shop. Go here to purchase item upgrades and other accessories\n");
                printf("\t(2) -- Open Achievements List\n");
                printf("(2) -- View your earned achievements and qualifiers to earn them\n");
                printf("\t(3) -- Save Game\n");
                printf("(3) -- Saves your game so you can open it later!\n");
                printf("\t(4) -- Rename Player\n");
                printf("(4) -- Renames the player on your save. This will cost 25 coins.\n");
                printf("\t(5) -- Exit\n");
                printf("(5) -- Exits the game. Goodbye!!!\n");
                scanf("%d", &c);
                while ((getchar()) != '\n');
                if (c==5)
                {
                    printf("\tThanks for playing!!\n");
                    printf("You reached level %.0f! You have earned a total of %d achievements!\n", state.rerollNumber, state.achievementsCounter);
                    getchar();
                    int makeSureSave;
                    int choice;
                    for(makeSureSave=1;makeSureSave>0;makeSureSave++)
                    {
                        ClearScreen();
                        printf("Save Game Before Exit?\n");
                        printf("(0) -- No\n");
                        printf("(1) -- Yes\n");
                        scanf("%d",&choice);
                        while ((getchar()) != '\n');
                        if(choice == 1)
                        {
                            saveGameState(filename,&state);
                            printf("Game Successfully Saved. Press enter to exit");
                            getchar();
                            break;
                        }
                        else if (choice == 0)
                        {
                            printf("Game not saved. Goodbye!");
                            getchar();
                            break;
                        }
                        else
                        {
                            printf("Invalid. Press enter to return to prompt");
                            getchar();
                        }
                    }
                    return 0;
                }
                else if(c==4)
                {
                    ClearScreen();
                    printf("Do you want to rename your character? This will cost you 10 coins. Enter 1 to confirm purchase\n");
                    int a;
                    int e;
                    for(a=1;a>0;a++)
                    {
                        scanf("%d",&e);
                        while ((getchar()) != '\n');
                        if(e==1 && state.coins >= 10)
                        {
                            printf("Awesome! Press Enter to confirm your purchase\n");
                            getchar();
                            state.coins-=10;
                            printf("Please type your new name\n");
                            scanf("%s", state.playerName);
                            while ((getchar()) != '\n');

                            printf("Awesome! Your name is now \033[93m%s\033[0m! Press enter to return to main menu\n",state.playerName);
                            getchar();
                            break;
                        }
                        else if(e==1 && state.coins < 10)
                        {
                            printf("You cannot afford name change. Press enter to return to main menu\n");
                            getchar();
                            break;
                        }
                        else
                        {
                            printf("Invalid input, returning to main menu\n");
                            break;
                        }
                    }
                }
                else if(c==3)
                {
                    int makeSureSave;
                    int choice;
                    for(makeSureSave=1;makeSureSave>0;makeSureSave++)
                    {
                        ClearScreen();
                        printf("Save Game?\n");
                        printf("(0) -- No\n");
                        printf("(1) -- Yes\n");
                        scanf("%d",&choice);
                        while ((getchar()) != '\n');
                        if(choice == 1)
                        {
                            saveGameState(filename,&state);
                            printf("Game Successfully Saved. Press enter to continue\n");
                            getchar();
                            break;
                        }
                        else if (choice == 0)
                        {
                            printf("Game Successfully Saved. Press enter to continue\n");
                            getchar();
                            break;
                        }
                        else
                        {
                            printf("Invalid. Press enter to return to prompt\n");
                            getchar();
                        }
                    }
                }
                else if(c==2)
                {
                    printf("Viewing Achievements. Press enter to continue.\n");
                    getchar();
                    ClearScreen();
                    printf("\t---Achievements List. Earned %d of 8---\n", state.achievementsCounter);
                    if(state._perfectionist)
                    {
                        printf("\033[90m(*) -- Perfectionist (Earned)*\033[0m\n");
                        printf("\033[90m-- In the rapier attack, wait a long time before using the attack*\033[0m\n");
                    } else
                    {
                        printf("(-) -- Perfectionist (Not Earned)\n");
                        printf("-- In the rapier attack, wait a long time before using the attack\n");

                    }
                    if(state._overkill)
                    {
                        printf("\033[90m(*) -- Overkill (Earned)*\033[0m\n");
                        printf("\033[90m-- Deal Over 60 Damage in a single attack*\033[0m\n");
                    } else
                    {
                        printf("(-) -- Overkill (Not Earned)\n");
                        printf("-- Deal Over 60 Damage in a single attack\n");

                    }
                    if(state._perfectTiming)
                    {
                        printf("\033[90m(*) -- Perfect Timing (Earned)*\033[0m\n");
                        printf("\033[90m-- Get a critical hit on the rapier attack*\033[0m\n");
                    } else
                    {
                        printf("(-) -- Perfect Timing (Not Earned)\n");
                        printf("-- Get a critical hit on the rapier attack\n");

                    }
                    if(state._perfectionist)
                    {
                        printf("\033[90m(*) -- Perfectionist (Earned)*\033[0m\n");
                        printf("\033[90m-- In the rapier attack, wait a long time before using the attack*\033[0m\n");
                    } else
                    {
                        printf("(-) -- Perfectionist (Not Earned)\n");
                        printf("-- In the rapier attack, wait a long time before using the attack\n");

                    }
                    if(state._ventureForth)
                    {
                        printf("\033[90m(*) -- Venture Forth (Earned)*\033[0m\n");
                        printf("\033[90m-- Make it past level 10*\033[0m\n");
                    } else
                    {
                        printf("(-) -- Venture Forth (Not Earned)\n");
                        printf("-- Make it past level 10\n");

                    }
                    if(state._trash)
                    {
                        printf("\033[90m(*) -- Trash (Earned)*\033[0m\n");
                        printf("\033[90m-- Lose in less than 3 turns*\033[0m\n");
                    } else
                    {
                        printf("(-) -- Trash (Not Earned)\n");
                        printf("-- Lose in less than 3 turns\n");

                    }
                    if(state._upgraded)
                    {
                        printf("\033[90m(*) -- Upgraded (Earned)*\033[0m\n");
                        printf("\033[90m-- Purchase your first upgrade in the shop*\033[0m\n");
                    } else
                    {
                        printf("(-) -- Upgraded (Not Earned)\n");
                        printf("-- Purchase your first upgrade in the shop\n");

                    }
                    if(state._overprepared)
                    {
                        printf("\033[90m(*) -- Overprepared (Earned)*\033[0m\n");
                        printf("\033[90m-- Acquire every upgrade and item in the shop.*\033[0m\n");
                    } else
                    {
                        printf("(-) -- Overprepared (Not Earned)\n");
                        printf("-- Acquire every upgrade item in the shop.\n");

                    }
                    if(state._bigbacked)
                    {
                        printf("\033[90m(*) -- BigBacked (Earned)*\033[0m\n");
                        printf("\033[90m-- Spend the entire duration of your turn eating food*\033[0m\n");
                    } else
                    {
                        printf("(-) -- BigBacked (Not Earned)\n");
                        printf("-- Spend the entire duration of your turn eating food\n");

                    }
                    printf("-----------------------------");
                    printf("Press enter to return to menu...");
                    getchar();
                }
                else if(c == 0)
                {
                    printf("Awesome. Travelling...\n");
                    rerolled = true;
                    getchar();
                    break;
                }
                else if(c == 1)
                {
                    printf("You have entered the shop. Press enter to continue\n");
                    getchar();
                    ClearScreen();

                    ShopText(&state);
                    int i;
                    for(i=1;i>0;i++)
                    {
                        if(!state.armorLevel1 && !state.armorLevel2) printf("\t(0) -- Armor Level 1 (20 coins)\n");
                        else if(state.armorLevel1 && !state.armorLevel2) printf("\t(0) -- Armor Level 2 (40 coins)\n");
                        else if(state.armorLevel1 && state.armorLevel2) printf("\t(0) -- Armor Maxed Out!!\n");
                        printf("(0) -- Provides protection against enemy attacks\n");
                        if(!state.sharpness1 && !state.sharpness2) printf("\t(1) -- Blade Sharpness 1 (25 coins)\n");
                        else if(state.sharpness1 && !state.sharpness2) printf("\t(1) -- Blade Sharpness 2 (50 coins)\n");
                        else if(state.sharpness1 && state.sharpness2) printf("\t(1) -- Blade Sharpness Maxed Out!!\n");
                        printf("(1) -- Provides more damage on your attacks\n");
                        if(!state.healthBoost1 && !state.healthBoost2) printf("\t(2) -- Health Boost 1 (35 coins)\n");
                        else if(state.healthBoost1 && !state.healthBoost2) printf("\t(2) -- Health Boost 2 (75 coins)\n");
                        else if(state.healthBoost1 && state.healthBoost2) printf("\t(2) -- Health Boost Maxed Out!!\n");
                        printf("(2) -- Raises your maximum health level. Also provides regeneration per every enemy encountered\n");
                        if(!state.hasRapier) printf("\t(3) -- Rapier (80 coins)\n");
                        else if(state.hasRapier) printf("\t(2) -- Rapier already bought!\n");
                        printf("(3) -- Unlocks a focused attack based on timing for accuracy.\n");


                        float foodPrice = 15.0f; // Base price
                        if (i > 4 && state.foodAmount > 4) {
                            foodPrice *= 1.24f; // 24% markup
                        }
                        if (i > 8 && state.foodAmount > 8) {
                            foodPrice *= 1.65f; // 65% markup
                        }

                        if(foodPrice > 15.0f) printf("\t(4) -- Food (%.2f coins **Supply and Demand Mark Up)**\n",foodPrice);
                        else printf("\t(4) -- Food (15 coins)\n");
                        printf("(4) -- Food items provide some healing!\n");
                        //Plan to add food like steak or something

                        printf("\t(5) -- Exit\n");
                        printf("(5) -- Exits the store\n");
                        printf("------------------------- ");
                        printf("Your Wallet: %.2f\n",state.coins);


                        if((state.sharpness1 || state.healthBoost1 || state.armorLevel1) && !state._upgraded)
                        {
                            AchievementGiver(Upgraded,&state);
                        }

                        if(state.sharpness1 && state.sharpness2 && state.healthBoost1 && state.healthBoost2 && state.armorLevel1 && state.armorLevel2 && state.hasRapier && state.foodAmount > 0 && !state._overprepared)
                        {
                            AchievementGiver(Overprepared,&state);
                        }


                        scanf("%d", &d);
                        while ((getchar()) != '\n');
                        if(d==0)
                        {
                            if(!state.armorLevel1 && !state.armorLevel2)
                            {
                                printf("\t(*) -- Press Enter to Confirm Purchase of Armor Level 1\n");
                                getchar();

                                if(state.coins >= 20)
                                {
                                    state.armorLevel1 = true;
                                    *storedCoins -= 20;
                                    ClearScreen();
                                    printf("\033[92mSuccessfully purchased Armor Level 1. Press Enter to Continue.\033[0m\n");

                                    ShopText(&state);
                                }
                                else
                                {
                                    ClearScreen();
                                    printf("\033[91mYou cannot afford this item. Press Enter to Continue\033[0m\n");
                                    getchar();

                                    ShopText(&state);
                                }
                            }
                            else if(state.armorLevel1 && !state.armorLevel2)
                            {
                                printf("\t(*) -- Press Enter to Confirm Purchase of Armor Level 2\n");
                                getchar();

                                if(state.coins >= 40)
                                {
                                    state.armorLevel2 = true;
                                    *storedCoins -= 40;
                                    ClearScreen();
                                    printf("\033[92mSuccessfully purchased Armor Level 2. Press Enter to Continue.\033[0m\n");

                                    ShopText(&state);
                                }
                                else
                                {
                                    ClearScreen();
                                    printf("\033[91mYou cannot afford this item. Press Enter to Continue\033[0m\n");
                                    getchar();

                                    ShopText(&state);
                                }
                            }
                            else
                            {
                                ClearScreen();
                                printf("This product is maxed out already!\n");
                            }
                        }
                        else if (d==1)
                        {
                            if(!state.sharpness1 && !state.sharpness2)
                            {
                                printf("\t(*) -- Press Enter to Confirm Purchase of Blade Sharpness 1\n");
                                getchar();

                                if(state.coins >= 25)
                                {
                                    state.sharpness1 = true;
                                    *storedCoins -= 25;
                                    ClearScreen();
                                    printf("\033[92mSuccessfully purchased Blade Sharpness 1. Press Enter to Continue.\033[0m\n");

                                    ShopText(&state);
                                }
                                else
                                {
                                    ClearScreen();
                                    printf("\033[91mYou cannot afford this item. Press Enter to Continue\033[0m\n");
                                    getchar();

                                    ShopText(&state);
                                }
                            }
                            else if(state.sharpness1 && !state.sharpness2)
                            {
                                printf("\t(*) -- Press Enter to Confirm Purchase of Blade Sharpness 2\n");
                                getchar();

                                if(state.coins >= 50)
                                {
                                    state.sharpness2 = true;
                                    *storedCoins -= 50;
                                    ClearScreen();
                                    printf("\033[92mSuccessfully purchased Blade Sharpness 2. Press Enter to Continue.\033[0m\n");

                                    ShopText(&state);
                                }
                                else
                                {
                                    ClearScreen();
                                    printf("\033[91mYou cannot afford this item. Press Enter to Continue\033[0m\n");
                                    getchar();

                                    ShopText(&state);
                                }
                            }
                            else
                            {
                                ClearScreen();
                                printf("This product is maxed out already!\n");
                            }

                        }
                        else if(d==2)
                        {
                            if(!state.healthBoost1 && !state.healthBoost2)
                            {
                                printf("\t(*) -- Press Enter to Confirm Purchase of Health Boost 1\n");
                                getchar();

                                if(state.coins >= 35)
                                {
                                    state.healthBoost1 = true;
                                    *storedCoins -= 35;
                                    state.playerHealth += 25;
                                    ClearScreen();
                                    printf("\033[92mSuccessfully purchased Health Boost 1. Press Enter to Continue.\033[0m\n");

                                    ShopText(&state);
                                }
                                else
                                {
                                    ClearScreen();
                                    printf("\033[91mYou cannot afford this item. Press Enter to Continue\033[0m\n");
                                    getchar();

                                    ShopText(&state);
                                }
                            }
                            else if(state.healthBoost1 && !state.healthBoost2)
                            {
                                printf("\t(*) -- Press Enter to Confirm Purchase of Health Boost 2\n");
                                getchar();

                                if(state.coins >= 75)
                                {
                                    state.healthBoost2 = true;
                                    *storedCoins -= 75;
                                    state.playerHealth += 35;
                                    ClearScreen();
                                    printf("\033[92mSuccessfully purchased Health Boost 2. Press Enter to Continue.\033[0m\n");

                                    ShopText(&state);
                                }
                                else
                                {
                                    ClearScreen();
                                    printf("\033[91mYou cannot afford this item. Press Enter to Continue\033[0m\n");
                                    getchar();

                                    ShopText(&state);
                                }
                            }
                            else if(state.healthBoost1 && state.healthBoost2)
                            {
                                ClearScreen();
                                printf("\033[91mThis product is already maxed out!\033[0m\n");
                                getchar();

                                ShopText(&state);
                            }
                        }
                        else if(d==3)
                        {
                            if(!state.hasRapier)
                            {
                                printf("\t(*) -- Press Enter to Confirm Purchase of Rapier\n");
                                getchar();

                                if(state.coins >= 80)
                                {
                                    ClearScreen();
                                    state.hasRapier = true;
                                    *storedCoins -= 80;
                                    printf("\033[92mSuccessfully purchased Rapier. Press Enter to Continue.\033[0m\n");

                                    ShopText(&state);
                                }
                                else
                                {
                                    ClearScreen();
                                    printf("\033[91mYou cannot afford this item. Press Enter to Continue\033[0m\n");
                                    getchar();

                                    ShopText(&state);
                                }
                            }
                            else if(state.hasRapier)
                            {
                                ClearScreen();
                                printf("\033[91mThis product is already maxed out!\033[0m\n");
                                getchar();

                                ShopText(&state);
                            }
                        }
                        else if(d==4)
                        {
                            printf("\t(*) -- Press Enter to Confirm Purchase of Food \n");
                            getchar();

                            if(state.coins >= 15*((i>4&&state.foodAmount>4) ? 1.24 : 1)*((i>8&&state.foodAmount>8) ? 1.33 : 1))
                            {
                                ClearScreen();
                                *storedCoins -= (15*((i>4&&state.foodAmount>4) ? 1.12 : 1)*((i>8&&state.foodAmount>8) ? 1.33 : 1));
                                *storedFood += 1;
                                printf("\033[92mSuccessfully purchased Food. Press Enter to Continue.\033[0m\n");

                                ShopText(&state);
                            }
                            else
                            {
                                ClearScreen();
                                printf("\033[91mYou cannot afford this item. Press Enter to Continue\033[0m\n");
                                getchar();

                                ShopText(&state);
                            }
                        }
                        else if(d==5)
                        {
                            ClearScreen();
                            printf("FareWell. Press Enter to Exit...\n");
                            getchar();
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            if (!rerolled)
            {
                if (turns%2 == 0)
                {
                    ClearScreen();
                    printf("Enemy's Turn Reached.\n");
                    getchar();

                    for(tries = 0; tries < 3; tries++)
                    {
                        printf("Your Health is %.2f/%.2f. The enemy's health is %.2f\n", state.playerHealth, state.maxHealth, enemyHealth);
                        printf("\n");
                        printf("What do you plan to do?\n");
                        printf("\t(0) -- Dodge\n");
                        printf("(0) -- Tries to evade the attack.\n");
                        printf("\t(1) -- Parry\n");
                        printf("(1) -- Has a 50/50 chance of success. You will take no damage and reflect some back to enemy upon success.\nYou will take 32%% more of the enemy attack damage upon failure if they land an attack.\n");
                        printf("\t(2) -- Endure\n");
                        printf("(2) -- Attempts to block and resist the enemy attack, reducing enemy attack damage upon success.\n");
                        scanf("%d", &b);
                        while ((getchar()) != '\n');

                        if(tries == 3)
                        {
                            printf("You ran out of tries. Press any key to continue\n");
                            state.playerHealth = Attack(1,state.playerHealth,1,state.armorResistance,1,&state);
                            getchar();

                            break;
                        }

                        if (b>=0 && b<3)
                        {
                            EvadeAction = DodgeOrParry(b);
                            switch(EvadeAction)
                            {
                            case Failed:
                                if(b==1) state.playerHealth = Attack(1,state.playerHealth,4,state.armorResistance,1,&state);
                                else if(b==2) state.playerHealth = Attack(0,state.playerHealth,4,state.armorResistance,1,&state);
                                else state.playerHealth = Attack(0,state.playerHealth,1,state.armorResistance,1,&state);
                                break;
                            case Grazed:
                                if(b==2) state.playerHealth = Attack(0,state.playerHealth,2,state.armorResistance*0.8f,1,&state);
                                else state.playerHealth = Attack(0,state.playerHealth,2,state.armorResistance*0.4f,1,&state);
                                break;
                            case Succeed:
                                state.playerHealth = state.playerHealth;
                                if(b==1) enemyHealth = Attack(0,enemyHealth,3,1,0.3f,&state);
                                break;
                            }
                            printf("Press Enter To Continue");
                            getchar();



                            break;
                        }
                        else
                        {
                            printf("Unknown move. Try again!\n");
                            getchar();
                        }
                    }
                    //End of ForLoop
                }
                else
                {
                    ClearScreen();
                    printf("Its your turn!\n");
                    if(enemyHealth <= 0) enemyHealth = 0;
                    if(state.playerHealth <= 0) state.playerHealth = 0;
                    if(turns > 1) printf("\tEnemy current health is %.2f. Your health is at %.2f/%.2f. Press Enter to Continue\n",enemyHealth,state.playerHealth,state.maxHealth);
                    getchar();

                    int tries;
                    for(tries = 0; tries < 3; tries++)
                    {
                        printf("What Attack are you trying to do?\n");
                        printf("\t(0) -- Basic Attack - Sword\n");
                        printf("(0) -- Has a 28.5%% chance of missing with an equal chance of landing a critical hit\n");
                        printf("\t(1) -- Heavy Attack - Mace\n");
                        printf("(1) -- Has a 47.05%% chance of missing, but has a higher damage output. 11.7%% chance of landing a critical hit.\n");
                        if(state.hasRapier && !state.isRapierCooldown)printf("\t(2) -- Timed Attack - Rapier\n");
                        if(state.hasRapier && !state.isRapierCooldown)printf("(2) -- This attack relies on timing to get the most damage!\n");
                        else if(state.hasRapier && state.isRapierCooldown)printf("\t\033[90m(2) -- Timed Attack - Rapier (On CoolDown)\033[0m\n");
                        printf("----------Inventory------------\n");
                        printf("This is your inventory. This contains various items that you can use.\n");
                        printf("Remember to choose an attack within your allotted tries!\n");
                        printf("-------------------------------\n");
                        printf("\t(*) -- Player Level - %.0f\n", state.rerollNumber);
                        printf("\t(*) -- Coins Amount - %.2f\n", state.coins);
                        if(state.foodAmount > 0)
                        {
                            printf("\t(10) -- Food x%d\n",state.foodAmount);
                            printf("(10) -- Heals some of your health upon consumption\n");
                        }
                        scanf("%d", &a);
                        while ((getchar()) != '\n');


                        if (a == 0)
                        {
                            printf("Awesome!! Attack Basic - Sword Selected\n");

                            printf("Press Enter to Attack!\n");
                            getchar();

                            enemyHealth = Attack(a,enemyHealth,0,1,1,&state);

                            printf("Enemy Health is now %.2f. Press enter to continue\n", enemyHealth);
                            getchar();

                            if(state.isRapierCooldown) state.isRapierCooldown = false;

                            break; //attack profile of 0 is the player

                        }
                        else if (a == 1)
                        {
                           printf("Awesome!! Heavy Attack - Mace Selected\n");

                           printf("Press Enter to Attack!\n");
                           getchar();

                           enemyHealth = Attack(a,enemyHealth,0,*amr,1,&state);

                           printf("Enemy Health is now %.2f. Press enter to continue\n", enemyHealth);
                           getchar();

                           if(state.isRapierCooldown) state.isRapierCooldown = false;
                           break;
                        }
                        else if(a == 2 && state.hasRapier && !state.isRapierCooldown)
                        {
                            printf("Awesome!! Timed Attack - Rapier Selected!\n");
                            float dealtDamage;
                            getchar();
                            t=0;
                            while(1)
                            {
                                frameIndex = (int)((sin(t)+1)*(numFrames - 1)/2);
                                frameIndex = frameIndex % numFrames;

                                ClearScreen();
                                printf("%s\n",framesAttack[frameIndex]);
                                printf("Time your attack! Press any key to attack.\n");

                                delay(40);

                                if(isKeyPressed())
                                {
                                    #ifdef _WIN32
                                    _getch(); //Clear the keypress buffer
                                    #else
                                    getchar();
                                    #endif // _WIN32
                                    int middleIndex = numFrames / 2;
                                    dealtDamage = (30 - abs(frameIndex - middleIndex)*(40/middleIndex));
                                    ClearScreen();
                                    if (dealtDamage < 12)
                                    {
                                        printf("\033[91m%s\033[0m\n",framesAttack[frameIndex]);
                                        printf("You missed!\n");
                                        state.isRapierCooldown = true;
                                    }
                                    else if (dealtDamage >= 12 && dealtDamage < 25)
                                    {
                                        printf("\033[93m%s\033[0m\n",framesAttack[frameIndex]);
                                        float damage = dealtDamage*0.8*(state.sharpness1 ? 1.2 : 1)*(state.sharpness2 ? 1.25 : 1);
                                        enemyHealth -= damage;
                                        printf("You attacked and dealt %.2f damage!\n",damage);

                                        if(t>=30 && !state._perfectionist)
                                        {
                                            AchievementGiver(Perfectionist,&state);
                                        }
                                        if(damage >= 60 && !state._overkill)
                                        {
                                            AchievementGiver(Overkill,&state);
                                        }
                                        state.isRapierCooldown = true;
                                    }
                                    else if (dealtDamage >= 25)
                                    {
                                        printf("\033[1;92m%s\033[0m\n",framesAttack[frameIndex]);
                                        float damage = dealtDamage*1.75*(state.sharpness1 ? 1.2 : 1)*(state.sharpness2 ? 1.25 : 1);
                                        enemyHealth -= damage;
                                        printf("CRITICAL HIT. You attacked and dealt %.2f damage, NICE TIMING!\n", damage);

                                        if(damage >= 60 && !state._overkill)
                                        {
                                            AchievementGiver(Overkill,&state);
                                        }
                                        if(!state._perfectTiming)
                                        {
                                            AchievementGiver(Perfect_Timing,&state);
                                        }
                                        if(t>=30 && !state._perfectionist)
                                        {
                                            AchievementGiver(Perfectionist,&state);
                                        }

                                        state.isRapierCooldown = true;
                                    }
                                    break;
                                }
                                t+=speed;
                            }

                            getchar();
                            break;
                        }

                        else if(a==10 && state.foodAmount>0)
                        {
                            if(state.playerHealth >= 20 + (state.healthBoost1 ? 25 : 0) + (state.healthBoost2 ? 35 : 0))
                            {
                                printf("Cannot eat, health already full.\n");
                                printf("You have %d tries left do do an attack\n", 2-tries);
                                getchar();
                            }
                            else
                            {
                                printf("Food Selected From Inventory. Press Enter to eat...\n");
                                getchar();
                                int randomHeal;
                                int min = 4;
                                int max = 10;
                                state.maxHealth = 20 + (state.healthBoost1 ? 25 : 0) + (state.healthBoost2 ? 35 : 0);
                                randomHeal = (rand() % (max-min + 1)) + min;

                                // Calculate the actual heal amount
                                float remainingHealth = state.maxHealth - state.playerHealth; // Health needed to reach max
                                float actualHeal = (randomHeal > remainingHealth) ? remainingHealth : randomHeal;

                                // Heal the player
                                state.foodAmount -= 1;
                                state.playerHealth += actualHeal;
                                state.bigbacktracker += 1;
                                printf("You ate and healed %.2f health. You are at %.2f/%.2f health\n", actualHeal, state.playerHealth, state.maxHealth);
                                printf("You have %d tries left do do an attack\n", (int)(2-tries));
                                getchar();
                            }
                        }
                        else
                        {
                            ClearScreen();
                            printf("Unknown action, try again!\n");
                        }

                        if(tries == 2)
                        {
                            ClearScreen();
                            if(state.bigbacktracker >= 3 && !state._bigbacked)
                            {
                                AchievementGiver(BigBacked,&state);
                            }
                            state.bigbacktracker = 0;
                            printf("You ran out of tries. Press any key to continue\n");
                            getchar();

                            break;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void initializeState(GameState* state)
{
    state->playerHealth = 20;
    state->maxHealth = 20;

    state->coins = 0;
    state->armorLevel1=false;
    state->armorLevel2=false;
    state->sharpness1=false;
    state->sharpness2=false;
    state->healthBoost1=false;
    state->healthBoost2=false;
    state->hasRapier = false; //change back to false
    state->isRapierCooldown = false;
    state->armorResistance = 1;
    state->foodAmount = 0;
    state->bigbacktracker = 0;


    state->bladeShapnessMulti = 1;
    //Achievements
    state->_overkill = false;
    state->_perfectionist = false;
    state->_perfectTiming = false;
    state->_ventureForth = false;
    state->_trash = false;
    state->_upgraded = false;
    state->_overprepared = false;
    state->_bigbacked=false;

    strcpy(state->playerName, "defaultPlayer");
    state->rerollNumber = 0;
    state->achievementsCounter = 0;
}


void saveGameState(const char* filename, const GameState* state)
{
    FILE* file = fopen(filename, "wb");
    if(file == NULL)
    {
        printf("Error: File could not be opened");
        return;
    }
    fwrite(state, sizeof(GameState),1, file);
    fclose(file);
}

int loadGameState(const char* filename, GameState* state)
{
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return 0;
    }
    fread(state, sizeof(GameState), 1, file);
    fclose(file);
    return 1;
}



void AchievementGiver(enum AchievementsList Achievenment,GameState* state)
{
    printf("\t============================\n");
    printf("\t=     \033[92mAchievement Get!\033[0m     =\n");
    printf("\t============================\n");
    state->achievementsCounter +=1 ;
    switch(Achievenment)
    {
        case Overkill:
            printf("Achievement Title: Overkill\n");
            printf("Qualifier: Deal more than 60 damage in a single attack\n");
            getchar();
            printf("Reward: 10 coins\n");
            state->coins += 10;
            state->_overkill = true;
            getchar();
            break;
        case Perfectionist:
            printf("Achievement Title: Perfectionist\n");
            printf("Qualifier: In the rapier attack, wait a long time before using the attack\n");
            getchar();
            printf("Reward: 20 coins\n");
            state->coins += 20;
            state->_perfectionist = true;
            getchar();
            break;
        case Venture_Forth:
            printf("Achievement Title: Venture Forth\n");
            printf("Qualifier: Make it past level 10\n");
            getchar();
            printf("Reward: 20 coins\n");
            state->coins += 20;
            state->_ventureForth = true;
            getchar();
            break;
        case Perfect_Timing:
            printf("Achievement Title: Perfect Timing\n");
            printf("Qualifier: Get a critical hit in the rapier attack\n");
            getchar();
            printf("Reward: 15 coins\n");
            state->coins += 15;
            state->_perfectTiming = true;
            getchar();
            break;
        case Trash:
            printf("Achievement Title: Trash\n");
            printf("Qualifier: Lose in less than 3 turns\n");
            getchar();
            printf("Reward: 0.1 coins\n");
            state->coins += 0.1;
            state->_trash = true;
            getchar();
            break;
        case Upgraded:
            printf("Achievement Title: Upgraded\n");
            printf("Qualifier: Purchase your first upgrade\n");
            getchar();
            printf("Reward: 10 coins\n");
            state->coins += 10;
            state->_upgraded = true;
            getchar();
            break;
        case Overprepared:
            printf("Achievement Title: Overprepared\n");
            printf("Qualifier: Acquire every upgrade and item in the shop");
            getchar();
            printf("Reward: 40 coins\n");
            state->coins += 40;
            state->_overprepared = true;
            getchar();
            break;
        case BigBacked:
            printf("Achievement Title: BigBacked\n");
            printf("Qualifier: Spend your entire turn eating food");
            getchar();
            printf("Reward: 3 Food and 10 Coins\n");
            state->coins += 10;
            state->foodAmount += 3;
            state->_bigbacked = true;
            getchar();
            break;
    }
}


void delay(int miliseconds)
{
    #ifdef _WIN32
    Sleep(miliseconds);
    #else // WIN32
    usleep(miliseconds*1000);
    #endif // Everything Else
}

void ClearScreen()
{
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif // _WIN32
}


float clamp(float value, float min, float max)
{
    return fmax(min,fmin(value,max));
}

float Attack(int a, float subjectHealth, int attackProfile, float DamageReductionMultiplier, float EnemyResistanceMultiplier, GameState* state)
{
    float randomizeDamage;
    int max, min;
    float scalingFactor = (state->rerollNumber > 4 ? 1+sqrt(state->rerollNumber-4) : 1);
    if(attackProfile == 4)
    {
        min = 2;
        max = 20;
        //EnemyAttack --- Soon to implement enemy attack first
        randomizeDamage = (rand() % (max-min + 1)) + min;

        if(randomizeDamage <= 18)
        {
            printf("You have been attacked!\n");
            if(a==1) return subjectHealth-(floor(randomizeDamage*0.85f)*DamageReductionMultiplier*scalingFactor)*1.32;
            else return subjectHealth-floor(randomizeDamage*0.85f)*DamageReductionMultiplier*scalingFactor;
        }
        else
        {
            printf("Critical Hit Landed\n");
            if(a == 1)
            return subjectHealth-((randomizeDamage*1.2f)*DamageReductionMultiplier*scalingFactor)*1.32;
            else return subjectHealth-(floor(randomizeDamage*1.2f)*DamageReductionMultiplier*scalingFactor);
        }
    }
    else if(attackProfile == 3)
    {
        min = 3;
        max = 10;
        //EnemyAttackParried
        randomizeDamage = (rand() % (max-min + 1)) + min;
        return subjectHealth-(randomizeDamage*EnemyResistanceMultiplier)*scalingFactor*3;
    }
    else if(attackProfile == 2)
    {
        min = 3;
        max = 10;
        //EnemyAttackGrazed
        randomizeDamage = (rand() % (max-min + 1)) + min;
        return subjectHealth-(randomizeDamage*DamageReductionMultiplier)*scalingFactor/2;
    }
    else if(attackProfile == 1)
    {
        min = 2+(scalingFactor/2);
        max = 20+(scalingFactor/2);
        //EnemyAttack --- Soon to implement enemy attack first
        randomizeDamage = (rand() % (max-min + 1)) + min;

        if(randomizeDamage < 10+(scalingFactor/2))
        {
            printf("The enemy missed their attack!!\n");
            return subjectHealth;
        }
        else if(randomizeDamage >= 10+(scalingFactor/2) && randomizeDamage < 18+(scalingFactor/2))
        {
            printf("You have been attacked!\n");
            return subjectHealth-floor(randomizeDamage*0.85f)*DamageReductionMultiplier*scalingFactor;
        }
        else
        {
            printf("Critical Hit Landed\n");
            return subjectHealth-floor(randomizeDamage*1.2f)*DamageReductionMultiplier*scalingFactor;
        }
    }
    else if(attackProfile == 0)
    {
        if(a == 0)
        {
            //BasicAttack -- 20% chance for crit, 10% chance for miss
            min = 0;
            max = 7;
            randomizeDamage = (rand() % (max-min + 1)) + min;

            if(randomizeDamage < 2)
            {
                printf("LOL YOU MISSED!!! No damage was applied!\n");
                getchar();
                return subjectHealth;
            }
            else if(randomizeDamage >= 2 && randomizeDamage < 5)
            {
                printf("Your attack successfully dealt %.2f damage!!\n", randomizeDamage*EnemyResistanceMultiplier*(state->bladeShapnessMulti));
                getchar();
                return subjectHealth - randomizeDamage*EnemyResistanceMultiplier*(state->bladeShapnessMulti);
            }
            else
            {
                float tempDamage;
                tempDamage = (randomizeDamage * ((randomizeDamage*2)/(max == 0 ? 0 : max)))*EnemyResistanceMultiplier*(state->bladeShapnessMulti);
                printf("Critical HIT!! You landed an attack that dealt %.2f damage!!\n", tempDamage);
                getchar();
                if(tempDamage >= 60 && !state->_overkill)
                {
                    AchievementGiver(Overkill, state);
                }
                return subjectHealth - tempDamage;
            }
        }
        else if(a == 1)
        {
            min = 0;
            max = 17;
            randomizeDamage = (rand() % (max-min + 1)) + min;

            if(randomizeDamage < 8)
            {
                printf("LOL YOU MISSED!!! No damage was applied!\n");
                getchar();
                return subjectHealth;
            }
            else if(randomizeDamage >= 8 && randomizeDamage < 15)
            {
                float tempDamage = randomizeDamage*1.7*EnemyResistanceMultiplier*(state->bladeShapnessMulti);
                printf("Your attack successfully dealt %.2f damage!!\n", tempDamage);
                getchar();
                if(tempDamage >= 60 && !state->_overkill)
                {
                    AchievementGiver(Overkill, state);
                }
                return subjectHealth - tempDamage;
            }
            else
            {
                float tempDamage;
                tempDamage = (randomizeDamage * ((randomizeDamage*2.6)/(max == 0 ? 0 : max)))*EnemyResistanceMultiplier*(state->bladeShapnessMulti);
                printf("\033[92mCritical HIT!! You dealt %.2f damage!!\033[0m\n", tempDamage);
                getchar();
                if(tempDamage >= 60 && !state->_overkill)
                {
                    AchievementGiver(Overkill, state);
                }
                return subjectHealth - tempDamage;
            }
        }
    }
    return subjectHealth;
}



enum EvadeActionStatus DodgeOrParry(int b)
{
    float randomChance;
    int max, min;
    if(b == 0)
    {
        //Dodge
        min = 0;
        max = 20;
        randomChance = (rand() % (max-min + 1)) + min;

        if(randomChance < 8)
        {
            printf("Your Dodge was too predictable!!\n");
            getchar();

            return Failed;
        }
        else if(randomChance >= 8 && randomChance < 15)
        {
            printf("You successfully evaded the attack!!\n");
            getchar();

            return Succeed;
        }
        else
        {
            printf("You have moved out of the way, but you were also grazed by the attack\n");
            getchar();

            return Grazed;
        }
    }
    else if(b == 1)
    {
        min = 0;
        max = 20;
        randomChance = (rand() % (max-min + 1)) + min;

        if(randomChance < 10)
        {
            printf("Your parry was mistimed!\n");
            getchar();
            return Failed;
        }
        else
        {
            printf("\033[92mYou successfully parried the attack!!\033[0m\n");
            getchar();
            return Succeed;
        }
    }
    else if (b==2)
    {
        min = 0;
        max = 20;
        randomChance = (rand() % (max-min + 1)) + min;

        if(randomChance < 4)
        {
            printf("The attack was too strong and you took full damage!\n");
            getchar();
            return Failed;
        }
        else if (randomChance >= 4 && randomChance < 13)
        {
            printf("Enemy attack endured.\n");
            getchar();
            return Grazed;
        }
        else
        {
            printf("Enemy attack fully blocked!\n");
            return Succeed;
        }
    }
    return Failed;
}




void LoseGame(GameState* state)
{
    printf("\t==========================\n");
    printf("\t-        You died!       -\n");
    printf("\t=You were slain by enemy!=\n");
    printf("\t-                        -\n");
    printf("\t==========================\n");
    printf("\033[31m%s, you lost the battle. Possible skill issue?\033[0m\n",state->playerName);
}

float RandomizeEnemyHealth()
{
    srand(time(NULL));
    float storedHealth = (rand()%61)+20;
    if(storedHealth < 20)
    {
        storedHealth = 20;
    }
    else if(storedHealth > 80)
    {
        storedHealth = 80;
    }
    return storedHealth;
}

void WinGame(GameState* state)
{
    printf("\t==========================\n");
    printf("\t-         Victory!       -\n");
    printf("\t=  You killed the enemy! =\n");
    printf("\t-                        -\n");
    printf("\t==========================\n");
    printf("\033[32m%s, you were victorious!\033[0m\n",state->playerName);
}

void ShopText(GameState* state)
{
    printf("\t==========================\n");
    printf("\t-          Shop          -\n");
    printf("\t=Where More Things Can Be=\n");
    printf("\t-                        -\n");
    printf("\t==========================\n");
    printf("Absolutely No REFUNDS!! U COMIT U BUY!!\n");
    printf("\n");
    printf("Welcome to the Shop \033[93m%s\033[0m, where more things can be. Browse the options below\nand type the appropriate number to select.\n", state->playerName);
    printf("\t------------------------------\n");
    printf("\n");
}

