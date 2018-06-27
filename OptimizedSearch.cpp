#include<iostream>
#include<fstream>
#include<vector>
#include<map>
#include<cstring>
#include<ctime>
#include<iomanip>
#include<algorithm>
#include<cctype>

typedef std::vector<int> (*searchFunction)(std::vector<std::vector<int>> const &, std::vector<int> const &);
class Node
{
	public:
	std::map<int,Node*> Children;
	std::vector<int>	rowIndexes;
	
	Node() : Children(),rowIndexes(){}

};

std::vector<std::vector<int>> read(std::ifstream& fs)
{
	std::vector<std::vector<int>> ret = std::vector<std::vector<int>> ();
	if(fs.is_open())
	{
		fs.seekg(0,fs.end);
		int length = fs.tellg();
		fs.seekg(0, fs.beg);
		
		if(length >8)
		{
			int it ;
			fs.read(reinterpret_cast<char *>(&it),sizeof(it));
			int row = it;
			fs.read(reinterpret_cast<char *>(&it),sizeof(it));
			int column = it;
			for(int y = 0; y < row ; ++y)
			{
				std::vector<int> row = std::vector<int>();
				for(int x =0; x < column ; ++x)
				{
					fs.read(reinterpret_cast<char *>(&it),sizeof(it));
					row.push_back(it);
				}
				ret.push_back(row);
			}
		}
	}
	else
	{
		std::cout <<"Error" <<std::endl;	
	} 
	fs.close();
	return ret;
}
void printIntVec (std::vector<int> const & inVec)
{
	for (int it : inVec )
	{
		std::cout << it << "  ";	
	}
	std::cout << std::endl;	
}

std::vector<int> XORCipher (std::vector<int> const & inVec, int key)
{
	std::vector<int> ret = std::vector<int>();
	for(int it : inVec )
	{
		ret.push_back(it ^ key);
	}
	return ret;
}

std::vector<std::vector<int>> XORCipherTwoDimension (std::vector<std::vector<int>> const & in2DVec, int key)
{
	std::vector<std::vector<int>> ret = std::vector<std::vector<int>>();
	for(std::vector<int> row : in2DVec)
	{
		ret.push_back(XORCipher(row, key));
	}
	return ret;
}

Node* insertValue (Node* baseNode, std::vector<int> const & sequence, const int& rowIndex)
{
	Node* iterator = baseNode;
	for(int it : sequence)
	{
		if(baseNode->Children.find(it) != baseNode->Children.end())
		{
			iterator = baseNode->Children[it];
			iterator-> rowIndexes.push_back(rowIndex);
		}
		else
		{
			iterator->Children[it] = new Node();
			iterator = iterator->Children[it];
			iterator-> rowIndexes.push_back(rowIndex);	
		}
	}
	return iterator;
}

std::vector<int>  getRowIndex (Node* baseNode, std::vector<int> const & sequence)
{
	Node* iterator = baseNode;
	std::vector<int> ret = std::vector<int>();
	
	for(int it : sequence)
	{
		if(iterator->Children.find(it) != iterator->Children.end())
		{
			iterator =iterator->Children[it];
		}
		else
		{
			return ret;	
		}
	};
	return iterator->rowIndexes;
}


std::vector<int> searchSequence(std::vector<std::vector<int>> const & inMatrix, std::vector<int> const & sequence)
{
	// Setting up nodes
	std::vector<int> ret = std::vector<int>();
	Node* baseNode =  new Node();
	for (int y = 0; y< inMatrix.size(); ++y)
	{
		for (int j = 0; j <inMatrix[y].size(); ++j)
		{
			std::vector<int> row = std::vector<int>();
			for(int i = j; i < inMatrix[y].size(); ++i)
			{
				row.push_back(inMatrix[y][i]);
			}
			insertValue(baseNode, row, y);
		}
	}
	std::cout <<"beginning Time" <<std::endl;
	const clock_t begin_time = std::clock();
	//read nodes
	std::vector<int> rows = getRowIndex(baseNode,sequence);
	// return rows that are correct
	for(int index : rows)
	{
			ret.push_back(index);
	}
	std::cout <<"time : " << float(std::clock() - begin_time ) / CLOCKS_PER_SEC <<std::endl;
	return ret;
}

std::vector<int> searchUnordered(std::vector<std::vector<int>> const & inMatrix, std::vector<int> const & sequence)
{
	std::vector<int> ret = std::vector<int>();
	// Handle information processing
	std::map<int,std::map<size_t, int >> processedMap = std::map<int,std::map<size_t, int >> ();
	for (int y = 0; y < inMatrix.size();++y)
	{
		for(int i = 0; i < inMatrix[y].size(); ++i)
		{
			//if the value of thesequence is inside
			if(processedMap.find(inMatrix[y][i]) != processedMap.end())
			{
				//if the row index is already saved;
				if(processedMap[inMatrix[y][i]].find(y) != processedMap[inMatrix[y][i]].end())
				{
					++processedMap[inMatrix[y][i]][y];
				}
				else
				{
					processedMap[inMatrix[y][i]][y] = 1;	
				}
			}
			else
			{
				processedMap[inMatrix[y][i]] = std::map<size_t,int>();
				processedMap[inMatrix[y][i]][y] = 1;
			}
		}
	}
	//handle 
	std::cout <<"beginning Time" <<std::endl;
	const clock_t begin_time = std::clock();
	std::map<size_t,int> rowScore = std::map<size_t,int>();
	for(int it : sequence)
	{
		//if it exists
		if(processedMap.find(it) != processedMap.end())
		{
			for ( auto temp : processedMap[it])
			{
				if(temp.second > 0)
				{
					--temp.second;
					if(rowScore.find(temp.first) != rowScore.end())
					{
						++rowScore[temp.first];
					}
					else
					{
						rowScore[temp.first] = 1;	
					}
				}
			}
		}
		//if it doesn't exist automatically fail
		else 
			return ret;
	}
	//prepare to return
	for(auto it : rowScore)
	{
		if(it.second == sequence.size())
		{
			ret.push_back(it.first);	
		}
	}

	std::cout <<"time : " << float(std::clock() - begin_time ) / CLOCKS_PER_SEC <<std::endl;
	return ret;
}

std::vector<int> searchBestMatch(std::vector<std::vector<int>> const & inMatrix, std::vector<int> const & sequence)
{
	std::vector<int> ret = std::vector<int>();
	// Handle information processing
	std::map<int,std::map<size_t, int >> processedMap = std::map<int,std::map<size_t, int >> ();
	for (int y = 0; y < inMatrix.size();++y)
	{
		for(int i = 0; i < inMatrix[y].size(); ++i)
		{
			//if the value of thesequence is inside
			if(processedMap.find(inMatrix[y][i]) != processedMap.end())
			{
				//if the row index is already saved;
				if(processedMap[inMatrix[y][i]].find(y) != processedMap[inMatrix[y][i]].end())
				{
					++processedMap[inMatrix[y][i]][y];
				}
				else
				{
					processedMap[inMatrix[y][i]][y] = 1;	
				}
			}
			else
			{
				processedMap[inMatrix[y][i]] = std::map<size_t,int>();
				processedMap[inMatrix[y][i]][y] = 1;
			}
		}
	}
	//handle 
	std::cout <<"beginning Time" <<std::endl;
	const clock_t begin_time = std::clock();
	std::map<size_t,int> rowScore = std::map<size_t,int>();
	for(int it : sequence)
	{
		//if it exists
		if(processedMap.find(it) != processedMap.end())
		{
			for ( auto temp : processedMap[it])
			{
				if(temp.second > 0)
				{
					--temp.second;
					if(rowScore.find(temp.first) != rowScore.end())
					{
						++rowScore[temp.first];
					}
					else
					{
						rowScore[temp.first] = 1;	
					}
				}
			}
		}
	}
	//prepare to return
	std::vector<int> rowIndexes = std::vector<int> ();
	int highestScore = 0;
	int retIndex = 0;
	for(auto it : rowScore)
	{
		if(it.second > highestScore)
		{
			retIndex = it.first;	
		}
	}
		std::cout <<"time : " << float(std::clock() - begin_time ) / CLOCKS_PER_SEC <<std::endl;
	ret.push_back(retIndex);
	return ret;
}

int main(int argc, char ** argv)
{
	
	if(argc < 3)
	{
		std::cout <<"Use case : ./SearchProgram matrix.dat searchSequence x x x" <<std::endl;	
		return -1;
	}
	// read in datfile name
	std::string fileName(argv[1]);
	if(fileName.find(".dat") == -1)
	{
		std::cout <<"Please specify valid .dat file" << std::endl;
		return -1;	
	}
	
	std::ifstream fs;
	fs.open (fileName, std::fstream::binary);
	if( !fs.is_open())
	{
		std::cout <<"Please specify valid .dat file" << std::endl;
		return -1;	
	}
	//Initialize search function map
	std::map<std::string,searchFunction> funcMap = std::map<std::string,searchFunction>();
	funcMap["searchsequence"] = searchSequence;
	funcMap["searchunordered"] = searchUnordered;
	funcMap["searchbestmatch"] = searchBestMatch;
	//get searchfunction
	std::string searchString = std::string(argv[2]);
	std::transform(searchString.begin(),searchString.end(),searchString.begin(),
				  [](unsigned char c) -> unsigned char { return std::tolower(c); });
	if(funcMap.find(searchString) == funcMap.end())
	{
		std::cout<<"search function named wrongly"<<std::endl;
		return -1;
	}
	
	std::vector<std::vector<int>>readVec  = read(fs);
	readVec = XORCipherTwoDimension(readVec,12345);
	/*
	for (std::vector<int> row : readVec)
	{
			printIntVec(row);
	}
	
	readVec = XORCipherTwoDimension(readVec,12345);
	for (std::vector<int> row : readVec)
	{
			printIntVec(row);
	}
	*/
	std::cout << std::fixed << std::showpoint;
	std::cout << std::setprecision(10);
	std::vector<int> vecSequence = std::vector<int>();
	for(int i = 3 ; i < argc ; ++i)
	{
		vecSequence.push_back(atoi(argv[i]));
	}
	std::vector<int> foundRowIndexes = funcMap[searchString](readVec,vecSequence);
	std::cout << "Below are the list of row indexes that match the search" <<std::endl;
	printIntVec(foundRowIndexes);
	return 0;
}
