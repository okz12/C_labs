#include "sudokusolver.h"

bool crosscheck(int board [][9],int row, int col){//this function checks the row, column and grid to see if the applied value creates a conflict
	int a,b;
	int rstart,cstart;
	for(a=0;a<9;a++){//To check columns and rows
		if((a!=row && board[a][col]==board[row][col])||(a!=col && board[row][a]==board[row][col]))//checks all columns and rows except its own hence a!=row and a!=col condition
			return false;//returns false if there is a conflict
	}
	rstart=(row/3)*3;//for a grid the starting row and column number need to be defined, in integer math the result rounds down, multiplied by 3 gives the starting point
	cstart=(col/3)*3;
	for(a=rstart;a<rstart+3;a++){
		for(b=cstart;b<cstart+3;b++){
			if(a!=row && b!=col && board[a][b]==board[row][col])//checks the grids for all values except its own, hence a!=row && b!=col condition
				return false;
		}
	}
	return true;//if false hasn't been returned in previous checks, there must be no conflict so return true
}
	
bool nextsolve(int board[][9],int row,int col){
	if(row==9)//condition for when end of puzzle is reached, as there are only 0-8 rows, when row=9, all blocks have been solved
		return true;
	int a;
	int nextrow,nextcol;//to find next row and column numbers to solve
	nextrow=row;
	nextcol=col+1;
	if(nextcol==9){//as there are only columns 0-8, 9 means that position 0 of next row should be used
		nextcol=0;
		nextrow=row+1;
	}
	if(board[row][col]!=0)//if the square has a prefilled or definite value, move on to solve next square
		return(nextsolve(board,nextrow,nextcol));//if false is received it will go back to the previous square
			//a difference here and the function that follows is we do not want to set the value of current block to 0 as it is pre-defined
	for(a=1;a<10;a++){//if the square does not have a definite value, try all values from 1-10
		board[row][col]=a;//assign value
		if(crosscheck(board,row,col)){//check if it causes any conflict, if not try and solve the next square
			if(nextsolve(board,nextrow,nextcol))
				return true;
		}
	}
	board[row][col]=0;//if after trying values 1-10, nothing fits, the current square is set to 0
	return false;//and false returned
}

void printboard(int board[][9]){//simple function for printing sudoku board
	int i,j;
	for (i=0;i<9;i++){
		if(i==3 || i==6)//produce a gap after row 3 and 6, to divide board into sub-grids
		cout<<endl;
		for (j=0;j<9;j++){
			if(j==3 || j==6)//produce a gap after column 3 and 6, to divide board into sub-grids
				cout<<" ";
			if(board[i][j]==0)
				cout<<"_ ";//if it holds the value 0, an underscore "_" is output instead of 0
			else
				cout<<board[i][j]<<" ";
		}
		cout<<endl;
	}
}

void printfile(int board[][9]){//function to write solved puzzle to a file
	int i,j;
	string filename;
	cout<<"Enter the filename with extension you wish to save to"<<endl;
	cin>>filename;//user enters filename to save to
	ofstream outfile(filename.c_str());
	for (i=0;i<9;i++){
		if(i==3 || i==6)//produce a gap after row 3 and 6, to divide board into sub-grids
		outfile<<endl;
		for (j=0;j<9;j++){
			if(j==3 || j==6)//produce a gap after column 3 and 6, to divide board into sub-grids
				outfile<<" ";
			if(board[i][j]==0)
				outfile<<"_ ";//if it holds the value 0, an underscore "_" is output instead of 0
			else
				outfile<<board[i][j]<<" ";
		}
		outfile<<endl;
	}
	outfile.close();
}

void readfile(int board[][9]){//function to read from file, file name is input
	string filename;
	int i,j;
	bool valid=false;
	while(!valid){//a simple  check is carried out to see if the sudoku board entered is valid, once file has been read, by seeing that no row, column or sub-grid have any number repeated
		cout <<"Please enter the name of your file with its extension" << endl;
		cin >> filename;
		ifstream infile(filename.c_str());//filename entered is opened
		if(!infile.is_open()){
			cout << "could not open file" << endl;
			exit (1);
		}
		for (i=0;i<9;i++){//loop to read int board array
			for (j=0;j<9;j++){
				infile>>board[i][j];
			}
		}
		infile.close();
		valid=true;
		for(i=0;i<9;i++){
			for(j=0;j<9;j++){
				if(!crosscheck(board,i,j) && board[i][j]!=0 && valid){//check to see for conflicts. We do not want to check value 0 (representing empty space) for conflict.
				//Once the sudoku is known to be invalid, further checking is unnecessary, hence valid condition.
					valid=false;
					cout<<"The file contains an invalid sudoku. Please try a different file."<<endl;//if there is a conflict, user will be asked to enter another filename
				}
			}
		}
	}
}

void readinput(int board[][9]){//function to take input from user
	bool satisfied=false;
	bool end;
	string s;
	bool input;
	int i,j,v;
	for (i=0;i<9;i++){
		for (j=0;j<9;j++){
		board[i][j]=0;
		}
	}
	cout<<"Fill in your sudoku in the format: row column value(e.g. 115 to fill 5 in top left hand corner).\nTo remove, enter value 0.\nTo finish, enter 'e'.\nTo print current board, enter 'p'."<<endl;
	end=false;
	while(!end){
		input=false;
		while(!input){
			cin>>s;
			if(((s=="p" || s=="e")) || s.size()==3){//user should enter a 3-digit string or p or e
				if(s[0]!='p' && s[0]!='e'){
					i=(int)s[0]-48;
					j=(int)s[1]-48;
					v=(int)s[2]-48;
					if((i<10 && i>0 && j<10 && j>0 && v<10 && v>=0))//checking the three digits are in the correct range
						input=true;
					else
						cout<<"Please enter data in correct format."<<endl;
				}
				else//if p or e has been entered
					input=true;
				}
			else
				cout<<"Please enter data in correct format."<<endl;
		}

		if(s[0]=='e'){//e to end
			printboard(board);
			cout<<"Is the sudoku correct? [y/n]"<<endl;//gives the user a chance to re enter sudoku
			cin>>s;
			if(s=="y")
				end=true;
		}
		else if(s[0]=='p')//to display sudoku board
			printboard(board);
			else{
				board[i-1][j-1]=v;//assign the value
				if(!crosscheck(board,i-1,j-1) && v!=0){//if the value entered conflicts with another value in the same row, column or subgrid, it reports an error and makes the square undefined
					board[i-1][j-1]=0;
					cout<<"Input value conflicts with a previous input. Please enter a valid sudoku"<<endl;
				}
			}
	}			
}