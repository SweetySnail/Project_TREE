#include "Manager.h"
Manager::Manager(int bpOrder) 
{
	
}

Manager::~Manager() 
{
	// delete tree for data
	delete bp;
	delete avl;
}

void Manager::run(const char* command_txt) 
{
	// open and read IN_Command text file
	ifstream read("command_list.txt");

	string cmd;
	// read the text file a line until file end
	while (getline(read, cmd))
	{
		char cmdbuff[512];
		// change string type to char type and copy in cmdbuff
		strcpy(cmdbuff, cmd.c_str());  // c_str -> string형을 char형으로 변환

		char* token1 = strtok(cmdbuff, " ");  // save command in token1
		char* token2 = strtok(NULL, "/");  // save values to need process command in token2
		char* token3 = strtok(NULL, "/");  // save values to need process SEARCH_CHARACTER function
		
		if (!strcmp(token1, "LOAD"))  // if token1 is LOAD
			LOAD(token2);  // process LOAD to use token2

		else if (!strcmp(token1, "UPDATE"))  // if token1 is UPDATE
			UPDATE(token2);  // process UPDATE to use token2

		else if (!strcmp(token1, "SEARCH_GAME"))  // if token1 is SEARCH_GAME
		{
			int num = atoi(token2);
			SEARCH_GAME(num);  // process SEARCH_GAME to use token2
		}

		else if (!strcmp(token1, "SEARCH_CHARACTER"))  // if token1 is SEARCH_CHARACTER
			SEARCH_CHARACTER(token2, token3);  // process SEARCH_CHARACTER to use token2

		else if (!strcmp(token1, "PRINT_GAME"))  // if token1 is PRINT_GAME
			PRINT_GAME();  // process PRINT_GAME to use token2

		else if (!strcmp(token1, "PRINT_CHARACTER"))  // if token1 is PRINT_CHARACTER
			PRINT_CHARACTER();  // process PRINT_CHARACTER to use token2

		else if (!strcmp(token1, "PRINT_PICK"))  // if token1 is PRINT_PICK
			PRINT_PICK();  // process PRINT_PICK to use token2

		else if (!strcmp(token1, "PRINT_WINRATIO"))  // if token1 is PRINT_WINRATIO
			PRINT_WINRATIO();  // process PRINT_WINRATIO to use token2
	}
		read.close();  // close read
}

bool Manager::LOAD(char* loadfile)
{
	if (strcmp(loadfile, "game_load.txt") != 0)  // if text file's name isn't same as "game_load.txt"
	{
		printErrorCode(100);  // Error: 100
		return false;
	}

	else  // if text file's name is same as "game_load.txt"
	{
		cout << "-----------------------------------------------------" << endl;
		cout << "                       LOAD" << endl;
		cout << "-----------------------------------------------------" << endl;
		cout << loadfile << endl;

		ofstream write("log.txt", ios::app);
		write << "-----------------------------------------------------" << endl;
		write << "                  PRINT CHARACTER" << endl;
		write << "-----------------------------------------------------" << endl;
		write << loadfile << endl;
		write.close();

		ifstream read("game_load.txt");
		
		char LOL[256];
		read.getline(LOL, 256);  // read line to useless line

		// make AVL Tree and B+ Tree
		bp = new BpTree();
		avl = new AVLTree;

		while (read.getline(LOL, 256))
		{
			// making Data Node(Gamedata, Characterdata)
			char cmd[512];
			strcpy(cmd, LOL);
			char* token;
			
			// make Game_Data
			Gamedata* Game_Data = new Gamedata;

			token = strtok(cmd, "	");  // insert id into Game_Data
			int id = atoi(token);
			Game_Data->setGameid(id);

			token = strtok(NULL, "	");  // insert duration into Game_Data
			int duration = atoi(token);
			Game_Data->setDuration(duration);

			token = strtok(NULL, "	");  // insert winner into Game_Data
			int winner = atoi(token);
			Game_Data->setWinner(winner);
			
			char* campion;
			// set TemaA
			for (int i = 0; i < 5; i++)
			{
				campion = strtok(NULL, "	");
				Game_Data->setTeamA(campion, i);

				Characterdata* Character_Data = new Characterdata;
				Character_Data->setname(campion);  // insert name into Character_Data
				Character_Data->setpickinc();  // insert pick into Character_Data
				if (winner == 0)
					Character_Data->setwininc();  // insert win into Character_Data
				bp->Insert(Character_Data);  // insert Character_Data into B+ Tree
			}
			//set TeamB
			for (int i = 0; i < 5; i++)
			{
				campion = strtok(NULL, "	");
				Game_Data->setTeamB(campion, i);

				Characterdata* Character_Data = new Characterdata;
				Character_Data->setname(campion);  // insert name into Character_Data
				Character_Data->setpickinc();  // insert pick into Character_Data
				if (winner == 1)
					Character_Data->setwininc();  // insert win into Character_Data
				bp->Insert(Character_Data);  // insert Character_Data into B+ Tree
			}
			avl->Insert(Game_Data);  // insert Game_Data into AVL Tree
		}
		return true;//file load complete
	}
}

bool Manager::UPDATE(char* updatefile) 
{
	ifstream read(updatefile);

	if (!read)  // if there's no file to read
	{
		printErrorCode(200);  // Error: 200
		return false;
	}
		
	else
	{
		if (avl == NULL || bp == NULL)  // if Tree's are empty
		{
			printErrorCode(200);  // Error: 200
			return false;
		}

		char cmd[256];
		read.getline(cmd, 256);

		while (read.getline(cmd, 256))
		{
			char cmdbuff[512];
			strcpy(cmdbuff, cmd);
			char* token;

			Gamedata* Update_Game = new Gamedata;
			
			token = strtok(cmdbuff, "	");
			int id = atoi(token);
			Update_Game->setGameid(id);  // insert id into Update_Game

			token = strtok(NULL, "	");
			int duration = atoi(token);
			Update_Game->setDuration(duration);  // insert duration into Update_Game

			token = strtok(NULL, "	");
			int winner = atoi(token);
			Update_Game->setWinner(winner);  // insert winner into Update_Game

			Gamedata* before = avl->Search(id);  // find Node

			char* character;
			for (int i = 0; i < 5; i++)
			{
				character = strtok(NULL, "	");
				Update_Game->setTeamA(character, i);
				
				Characterdata* Update_Character = new Characterdata;
				Update_Character->setname(character);  // insert name into Update_Character
				Update_Character->setpickinc();  // insert win into Update_Character
				if (winner == 0)
					Update_Character->setwininc();  // insert Character_Data into Update_Character

				// edit before TeamA member information
				string Before_Character = before->getTeamA()[i];
				BpTreeNode* before_character = bp->searchDataNode(Before_Character);
				map<string, Characterdata*>::iterator it = before_character->getDataMap()->find(Before_Character);
				it->second->setpickdec();
				if (before->getWinner() == 0)
					it->second->setwindec();
													
				// edit new TeamA member information
				bp->Insert(Update_Character);
			}

			for (int i = 0; i < 5; i++)
			{
				character = strtok(NULL, "	");
				Update_Game->setTeamB(character, i);

				Characterdata* Update_Character = new Characterdata;
				Update_Character->setname(character);  // insert name into Update_Character
				Update_Character->setpickinc();  // insert win into Update_Character
				if (winner == 0)
					Update_Character->setwininc();  // insert Character_Data into Update_Character

				// edit before TeamB member information
				string Before_Character = before->getTeamB()[i];
				BpTreeNode* before_character = bp->searchDataNode(Before_Character);
				map<string, Characterdata*>::iterator it = before_character->getDataMap()->find(Before_Character);
				it->second->setpickdec();
				if (before->getWinner() == 1)
					it->second->setwindec();

				// edit new TeamB member information
				bp->Insert(Update_Character);
			}
			
			// update AVL Tree
			Gamedata* update = avl->Search(Update_Game->getGameid());  // find Node
			if (update == NULL)  // if there's no search Node
			{
				printErrorCode(200);  // Error: 200
				return false;
			}
			
			update->setDuration(Update_Game->getDuration());  // update duration
			update->setWinner(Update_Game->getWinner());  // update winner
			for (int i = 0; i < 5; i++)
			{
				update->setTeamA(Update_Game->getTeamA()[i], i);  // update TeamA
				update->setTeamB(Update_Game->getTeamB()[i], i);  // update TeamB
			}
		}
	}
	return true;//file update complete
}

bool Manager::SEARCH_GAME(int id1) 
{
	// save Data to search
	Gamedata* Search_Data = avl->Search(id1);

	if (Search_Data != NULL)  // if there's search data
	{
		// print Search_Data's information
		cout << "-----------------------------------------------------" << endl;
		cout << "                       SEARCH " << id1 << endl;
		cout << "-----------------------------------------------------" << endl;
		cout << "Gameid: " << Search_Data->getGameid() << endl;
		cout << "Duration: " << Search_Data->getDuration() << endl;
		cout << "Winner: " << Search_Data->getWinner() << endl;
		cout << "TeamA: ";
		for (int i = 0; i < 4; i++)
			cout << Search_Data->getTeamA()[i] << " / ";
		cout << Search_Data->getTeamA()[4] << endl;
		cout << "TeamB: ";
		for (int i = 0; i < 4; i++)
			cout << Search_Data->getTeamB()[i] << " / ";
		cout << Search_Data->getTeamB()[4] << endl;

		ofstream write("log.txt", ios::app);
		write << "-----------------------------------------------------" << endl;
		write << "                  SEARCH " << id1 << endl;
		write << "-----------------------------------------------------" << endl;
		write << "Gameid: " << Search_Data->getGameid() << endl;
		write << "Duration: " << Search_Data->getDuration() << endl;
		write << "Winner: " << Search_Data->getWinner() << endl;
		write << "TeamA: ";
		for (int i = 0; i < 4; i++)
			write << Search_Data->getTeamA()[i] << " / ";
		write << Search_Data->getTeamA()[4] << endl;

		for (int i = 0; i < 4; i++)
			write << Search_Data->getTeamB()[i] << " / ";
		write << Search_Data->getTeamB()[4] << endl;
		write.close();

		return true;//game search complete
	}
	else  // if there's no search data
	{
		printErrorCode(300);  // Error: 300
		return false;
	}
}

bool Manager::SEARCH_CHARACTER(char* Charactername1, char* Charactername2) 
{
	string first = Charactername1;
	string second = Charactername2;

	// if Charactername1 is bigger than Charactername2, change each other
	if (first > second)
	{
		string temp = second;
		second = first;
		first = temp;
	}

	BpTreeNode* Begin_Node = NULL;
	BpTreeNode* End_Node = NULL;

	Begin_Node = bp->searchDataNode(first);  // save Node's position that have first
	End_Node = bp->searchDataNode(second);  // save Node's position that have first

	if (Begin_Node == NULL || End_Node == NULL)  // if there's no Node
	{
		printErrorCode(400);  // Error: 400
		return false;
	}
		
	else  // if there's Node
	{
		cout << "-----------------------------------------------------" << endl;
		cout << "                  SEARCH " << first << " / " << second << endl;
		cout << "-----------------------------------------------------" << endl;
		cout << "Name	Pick	Win	WinRatio" << endl;
		
		ofstream write("log.txt", ios::app);
		write << "-----------------------------------------------------" << endl;
		write << "                  SEARCH" << first << " / " << second << endl;
		write << "-----------------------------------------------------" << endl;
		write << "Name	Pick	Win	WinRatio" << endl;
		write.close();

		bp->SearchRange(first, second);  // data print in range 
		return true;
	}
}

bool Manager::PRINT_GAME() 
{
	if (avl != NULL)  // if AVL Tree isn't empty
	{
		cout << "-----------------------------------------------------" << endl;
		cout << "                  PRINT GAME" << endl;
		cout << "-----------------------------------------------------" << endl;
		cout << "Gameid	Winner	Duration	TeamA_c1	TeamA_c2	TeamA_c3	TeamA_c4	TeamA_c5	TeamB_c1	TeamB_c2	TeamB_c3	TeamB_c4	TeamB_c5" << endl;

		ofstream write("log.txt", ios::app);
		write << "-----------------------------------------------------" << endl;
		write << "                  PRINT GAME" << endl;
		write << "-----------------------------------------------------" << endl;
		write << "Gameid	Winner	Duration	TeamA_c1	TeamA_c2	TeamA_c3	TeamA_c4	TeamA_c5	TeamB_c1	TeamB_c2	TeamB_c3	TeamB_c4	TeamB_c5" << endl;
		write.close();

		avl->Print();  // data print all of AVL Tree

		return true;//print Game in gameid ascending order(id	winner	duration	teama	teamb)
	}
	else  // if AVL Tree is empty
	{
		printErrorCode(500);  // Error: 500
		return false;
	}
}

bool Manager::PRINT_CHARACTER() 
{
	if (bp != NULL)  // if B+ Tree isn't empty
	{
		cout << "-----------------------------------------------------" << endl;
		cout << "                  PRINT CHARACTER" << endl;
		cout << "-----------------------------------------------------" << endl;
		cout << "Name	Pick	Win	WinRatio" << endl;

		ofstream write("log.txt", ios::app);
		write << "-----------------------------------------------------" << endl;
		write << "                  PRINT CHARACTER" << endl;
		write << "-----------------------------------------------------" << endl;
		write << "Name	Pick	Win	WinRatio" << endl;
		write.close();

		bp->Print();  // data print all of B+ Tree
		
		return true;//charater print complete
	}
	else  // if B+ Tree is empty
	{
		printErrorCode(600);  // Error: 600
		return false;
	}
		
}

bool Manager::PRINT_PICK() 
{
	if (bp != NULL)  // if B+ Tree isn't empty
	{
		ofstream write("log.txt", ios::app);

		cout << "-----------------------------------------------------" << endl;
		cout << "                   " << "PRINT PICK" << endl;
		cout << "-----------------------------------------------------" << endl;

		write << "-----------------------------------------------------" << endl;
		write << "                  " << "PRINT PICK" << endl;
		write << "-----------------------------------------------------" << endl;

		cout << "Name	Pick	Win	WinRatio" << endl;
		write << "Name	Pick	Win	WinRatio" << endl;

		bp->printPickRank();  // data print all of B+ Tree in pick rate's decending order 

		write.close();
		return true;//print complete(no print return false)
	}

	else  // if B+ Tree is empty
	{
		printErrorCode(700);  // Error: 700
		return false;
	}
	 
}

bool Manager::PRINT_WINRATIO() 
{
	if (bp != NULL)  // if B+ Tree isn't empty
	{
		ofstream write("log.txt", ios::app);

		cout << "-----------------------------------------------------" << endl;
		cout << "                   " << "PRINT WINRATIO" << endl;
		cout << "-----------------------------------------------------" << endl;

		write << "-----------------------------------------------------" << endl;
		write << "                  " << "PRINT WINRATIO" << endl;
		write << "-----------------------------------------------------" << endl;

		cout << "Name	Pick	Win	WinRatio" << endl;
		write << "Name	Pick	Win	WinRatio" << endl;

		bp->printRatioRank();  // data print all of B+ Tree in win rate's ascending order

		write.close();
		return true;//print complete(no print return false)
	}

	else  // if B+ Tree is empty
	{
		printErrorCode(800);  // Error: 800
		return false;
	}
		
}

void Manager::printErrorCode(int n) {
	cout << "======== ERROR ========" << endl;
	cout << n << endl;
	cout << "=======================" << endl;
	
	ofstream fout;
	fout.open("log.txt", ofstream::app);
	fout << "======== ERROR ========" << endl;
	fout << n << endl;
	fout << "=======================" << endl;
	fout.close();
}

