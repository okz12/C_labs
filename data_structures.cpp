/* Assignment 1
 * Author: Osman Khawer Zubair
 * Email: osman.zubair12@imperial.ac.uk
 * Login: okz12
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

using namespace std;

typedef int Item;//define Item type as integer, easier to change data type in future

struct Node{//Node consists of an Item and a pointer to node
	Item data;
	Node* next;
};

typedef Node* NodePtr;//define NodePtr type as pointer to node

void readFromFile(NodePtr &hdList, string filenumber);
void sortList (NodePtr &hdList);
void writeList (NodePtr hdList, string filenumber);
void insertItem (Item data, NodePtr &hdList);
void deleteItem (Item data, NodePtr &hdList);
void writeMaxValue(NodePtr hdList, string filenumber);
void writeItemsAboveT(NodePtr hdList, Item T, string filenumber);
void interpolateAverage(NodePtr hdList);
void addToList (Item number, NodePtr &hdList);
Item getFromList (NodePtr &hdList);
void printAll (NodePtr hdList);

int main (int argc, char *argv[]){//command line arguments held in argv
	bool sorted = false;
	char read;
	string filenumber;
	NodePtr hdList = NULL;//head of list points to NULL initially, which indicates end of list
	Item insertData, deleteData, T;
	ifstream commandfile;
		
	commandfile.open(argv[1]);//open commandfile using argument held in argv[1]
	if(!commandfile.is_open()){//exit if unable to open command file
		cout << "could not open command file" << endl;
		exit(EXIT_FAILURE);
	}
		
	while(commandfile >> read){//read commandfile until the end
	
		if (read=='r'){//read data_x.txt
			commandfile >> filenumber;
			readFromFile (hdList, filenumber);
		}
			
		else if (read=='s'){//bubble sort list
			sortList(hdList);
			sorted=true;
		}
		
		else if (read=='w')//write list to output file
			writeList (hdList, filenumber);
		
		else if (read=='i'){//insert element in list
			commandfile >> insertData;
			if (sorted)//if list is sorted add to appropriate position
				insertItem (insertData, hdList);
			else{//else add to head of the list
				addToList (insertData, hdList);
				cout<<insertData<<" inserted"<<endl;//print the value of the inserted element followed by the updated list
				printAll(hdList);
				cout<<endl;
				}
		}
		
		else if (read=='d'){//delete element from list
			commandfile >> deleteData;
			deleteItem (deleteData, hdList);
		}
		
		else if (read=='e'){//print number of elements in list above threshold (T) to file
			commandfile >> T;
			writeItemsAboveT(hdList, T, filenumber);
		}
		
		else if (read=='m')//print maximum value in list to file
			writeMaxValue(hdList, filenumber);
			
		else if (read=='a')//interpolate list with averages
			interpolateAverage(hdList);
	}
	
	commandfile.close();//close command file
	return 0;
}

void readFromFile(NodePtr &hdList, string filenumber){
	Item datain;
	string filename = "data_" + filenumber + ".txt";//building data_x.txt file name where x=filenumber
	ifstream datafile;
	datafile.open(filename.c_str());//open data_x.txt file
	if(!datafile.is_open()){//exit if file not open
		cout << "could not open data file" << endl;
		exit(EXIT_FAILURE);
	}

	while(datafile >> datain){//reads datafile and adds new data to head of list, also changes hdList to point to new data
		 addToList(datain, hdList);
	}
	
	datafile.close();//close data_x.txt file
	cout<<"Read from file: "<<filename<<endl;//print list
	printAll(hdList);
	cout<<endl;
}

void sortList (NodePtr &hdList){
	if(hdList == NULL){//return if list is empty
		cout<<"Sort List Error: List is empty"<<endl;
		return;
	}
	
	if(hdList->next == NULL){//return if list only contains one element
		cout<<"Sort List Error: List contains only one element"<<endl;
		return;
	}
	
	NodePtr nowPtr = NULL;//pointer to current node
	NodePtr prevPtr = NULL;//pointer to previous node
	NodePtr nextPtr = NULL;//pointer to next node
	bool unchanged = false;

	while(!unchanged){ //exit the sorting loop if one pass is made without any swaps
	unchanged = true;
	nowPtr = hdList;//nowPtr and prevPtr point to the beginning of the list
	prevPtr = hdList; 
	nextPtr = nowPtr->next;//nextPtr points to 2nd node

	while (nowPtr->next != NULL) { //while end of list is not reached
	  if (nowPtr->data > nextPtr->data) { //if current node data value is greater than next node's

		nowPtr->next = nextPtr->next;//current node points to the node after next
		nextPtr->next=nowPtr;//next node points to current node

		if(nowPtr == hdList){ //if the first element was swapped
			hdList = nextPtr;//head of list points to nextPtr node which was swapped into first place
			prevPtr = nextPtr;//prevPtr also points to the same node
		}
		else //if some other element was swapped
			prevPtr->next = nextPtr;//previous node points to nextPtr node which has been swapped into current position
		nowPtr = nextPtr; //nowPtr now points to current node (since nextPtr points to the node which was swapped into the current position)
		unchanged = false;//a swap has been made
	  }
	  prevPtr = nowPtr;//advance pointers
	  nowPtr = nowPtr->next;
	  nextPtr = nowPtr->next;
	}
	}
	cout<<"List sorted"<<endl;//print sorted list
	printAll(hdList);
	cout<<endl;
}

void writeList (NodePtr hdList, string filenumber){
	ofstream outfile;
	string filename;
	filename = "output_" + filenumber + ".txt";//building output_x.txt filename where x=filenumber
	outfile.open(filename.c_str(), fstream::app);//open output file in append mode
	if(!outfile.is_open()){//exit if file not open
		cout << "could not open output file" << endl;
		exit(EXIT_FAILURE);
	}
	while (hdList != NULL){//while end of list is not reached
		outfile << getFromList(hdList) << endl;//write data value to output_x.txt file, advance hdList
		}
	outfile.close();//close output_x.txt file
	cout<<"Wrote to file"<<endl<<endl;
}

void insertItem (Item data, NodePtr &hdList){
	bool found = false;
	NodePtr searchPtr, lastPtr, newPtr;
	
	newPtr = new Node;//create new node
	newPtr -> data = data;//assign data value to new node
	newPtr -> next = NULL;//new node points to NULL for now
	
	if (hdList==NULL){//if list is empty, head of list points to new node which points to NULL
		hdList = newPtr;
	}
	
	else if (hdList->data >= data){//if element needs to be inserted at head
		newPtr->next = hdList;//make new node point to hdList node
		hdList = newPtr;//then make head of list point to the new node
	}
	
	else{//in case of normal insertion
		searchPtr = hdList;//searchPtr and lastPtr set to beginning of list
		lastPtr = hdList;
		while((searchPtr != NULL) && (!found)){//while end of list is not reached or insertion node not found
			if(searchPtr->data >= data)//if searchPtr is greater than insertion data value, insertion area found
				found = true;
			else{
				lastPtr = searchPtr;//else advance pointers
				searchPtr = searchPtr->next;
			}
		}
	newPtr->next = searchPtr;//relinking into correct place, new node points to searchPtr
	lastPtr->next = newPtr;//lastPtr points to new node
	}
	
	cout<<data<<" inserted"<<endl;//print the value of the item inserted and the updated list
	printAll(hdList);
	cout<<endl;
}

void deleteItem (Item data, NodePtr &hdList){
	bool found = false;
	NodePtr searchPtr, lastPtr, oldPtr;
	
	if (hdList==NULL)//return if list is empty
		return;
		
	else if (hdList->data == data){//if data to be deleted is the first element, hdList needs to be changed
		oldPtr = hdList;//oldPtr points to hdList node
		hdList = hdList->next;//head of list points to next node
		delete oldPtr;//first element is deleted in this manner to prevent memory leaks
	}
	
	else{
		searchPtr = hdList;//searchPtr and lastPtr point to beginning of list initially
		lastPtr = hdList;
		while((searchPtr != NULL) && (!found)){//while list has not ended and element not found
			if(searchPtr->data==data){//if searchPtr data value equals the deletion data value
				found = true;//found
				lastPtr->next = searchPtr->next;//lastPtr node points to the node after searchPtr (skipping searchPtr)
				delete searchPtr;//delete searchPtr to prevent memory leak
			}
			else{//if searchPtr data value does not equal deletion data value, advance pointers
				lastPtr = searchPtr;//lastPtr points to node before searchPtr
				searchPtr = searchPtr->next;
			}
		}
	}
	
	cout<<data<<" deleted"<<endl;//print the value of the deleted element
	printAll(hdList);
	cout<<endl;
}

void writeItemsAboveT(NodePtr hdList, Item T, string filenumber){
	int count=0;//count is the number of items above threshold
	
	while (hdList != NULL){//while end of list is not reached
		if(getFromList(hdList) > T)//get data value and increment hdList, and if data value is above threshold increment count
			count++;
	}
	
	ofstream outfile;
	string filename;
	filename = "output_" + filenumber + ".txt";//building output_x.txt filename where x=filenumber
	outfile.open(filename.c_str(), fstream::app);//open output file in append mode
	if(!outfile.is_open()){//exit if file not open
		cout << "could not open output file" << endl;
		exit(EXIT_FAILURE);
	}
	outfile<<"Number  of  elements  with  value  greater  than  "<<T<<":  "<<count<<endl;//write to output_x.txt file
	outfile.close();//close output_x.txt file
	
	cout<<"Number  of  elements  with  value  greater  than  "<<T<<":  "<<count<<endl<<endl;//print number of items above T
}

void writeMaxValue(NodePtr hdList, string filenumber){
	if (hdList == NULL){//return if list is empty
		cout<<"Maximum Value Error: List is empty"<<endl;
		return;
	}
	
	int n;
	int max=getFromList(hdList);//first item is currently maximum, also advances hdList by one
	while (hdList != NULL){//while end of list is not reached
		n=getFromList(hdList);//get value of next element, advance hdList by one
		if(n > max)//if value is greater than max, assign this value to max
			max = n;
	}
	
	ofstream outfile;
	string filename = "output_" + filenumber + ".txt";//building output_x.txt filename where x=filenumber
	outfile.open(filename.c_str(), fstream::app);//open output file in append mode
	if(!outfile.is_open()){//exit if file not open
		cout << "could not open output file" << endl;
		exit(EXIT_FAILURE);
	}
	outfile<<"Maximum value:  "<<max<<endl;//write to output_x.txt file
	outfile.close();//close output_x.txt file
	
	cout<<"Maximum value: "<<max<<endl<<endl;//print the maximum value
}

void interpolateAverage(NodePtr hdList){
	NodePtr nowPtr, nextPtr, newPtr;
	
	if(hdList == NULL){//return if list is empty
		cout<<"Interpolate Error: List is empty"<<endl;
		return;
	}
	
	if(hdList->next == NULL){//return if list only contains one element
		cout<<"Interpolate Error: List contains only one element"<<endl;
		return;
	}
	
	nowPtr = hdList;//nowPtr starts at the beginning of list
	nextPtr = nowPtr -> next;
	
	while (nextPtr != NULL){
		newPtr = new Node;//newPtr points to a new empty node
		newPtr->data = (nowPtr->data + nextPtr->data)/2;//data value of newPtr is the average of data values of nowPtr and nextPtr
		newPtr->next = nextPtr;//linking the new node in, new node points to next node
		nowPtr->next = newPtr;//now node points to new node
		nowPtr = nextPtr;//advance pointers
		nextPtr = nextPtr->next;
	}
	
	cout<<"List interpolated"<<endl;//print the interpolated list
	printAll(hdList);
	cout<<endl;
}

void addToList (Item number, NodePtr &hdList){//add node to head of list
	NodePtr newNode = new Node;//declare new node
	newNode -> data = number;//assign data value to node
	newNode -> next = hdList;//new node points to current head of list
	hdList = newNode;//hdList points to new node
}

Item getFromList (NodePtr &hdList){//returns data value of node and advances pointer
	Item number;
	NodePtr nowPtr;
	nowPtr = hdList;
	number = nowPtr -> data;//extract data value
	hdList = nowPtr -> next;//advance pointer
	return Item(number);
}

void printAll (NodePtr hdList){//prints list from pointer till the end
	cout << "\nList of numbers:\n";
	while (hdList != NULL){
		cout << getFromList(hdList) << endl;//prints current item and advances pointer
	}
}
