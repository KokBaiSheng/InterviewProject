#include<iostream>
#include<fstream>
#include<vector>
#include<map>
#include<cstring>
#include<ctime>
#include<iomanip>
#include<algorithm>
#include<cctype>
#include<chrono>
#include<unordered_set>

class Timer
{
	public:
	std::chrono::microseconds GetDuration()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	}
	Timer()
	{
		StartTimer();	
	}
	void StartTimer()
	{
		start = std::chrono::high_resolution_clock::now();
	}
	void EndTimer()
	{
		end = std::chrono::high_resolution_clock::now();
	}
	private:
	std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point end;
};

typedef std::vector<int> (*searchFunction)(std::vector<std::vector<int>> const &, std::vector<int> const &, double &, bool const & );
class Node
{
	public:
	std::map<int,Node*> Children;
	std::unordered_set<int>	rowIndexes;
	
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

int insertValue (Node* baseNode, std::vector<int> const & sequence, const int& rowIndex)
{
	Node* iterator = baseNode;
	int ret = 0;
	for(int it : sequence)
	{
		if(iterator->Children.find(it) != iterator->Children.end())
		{
			iterator = iterator->Children[it];
			iterator-> rowIndexes.insert(rowIndex);
		}
		else
		{
			iterator->Children[it] = new Node();
			iterator = iterator->Children[it];
			iterator-> rowIndexes.insert(rowIndex);	
		}
		++ret;
	}
	return ret;
}

void deleteNodes(Node* baseNode)
{
	for (std::pair<const int,Node*>& elem : baseNode->Children)
	{
		deleteNodes(elem.second);
	}
	delete(baseNode);
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
	for(const auto& elem : iterator->rowIndexes)
	{
		ret.push_back(elem);	
	}
	return ret;
}
std::vector<int> searchSequenceSearch(Node*& baseNode, std::vector<int> const & sequence, double & accumulatedTime)
{
	std::vector<int> ret = std::vector<int>();
	Timer currTimer = Timer();
	//read nodes
	std::vector<int> rows = getRowIndex(baseNode,sequence);
	// return rows that are correct
	for(int index : rows)
	{
			ret.push_back(index);
	}
	currTimer.EndTimer();
	accumulatedTime += currTimer.GetDuration().count();
	return ret;
}

std::vector<int> searchSequence(std::vector<std::vector<int>> const & inMatrix, std::vector<int> const & sequence, double & accumulatedTime, bool const & isBenchmarking)
{
	// Setting up nodes
	std::vector<int> ret = std::vector<int>();
	Node* baseNode =  new Node();
	long nodenum = 0;
	for (int y = 0; y< inMatrix.size(); ++y)
	{
		for (int j = 0; j <inMatrix[y].size(); ++j)
		{
			std::vector<int> row = std::vector<int>();
			for(int i = j; i < inMatrix[y].size(); ++i)
			{
				row.push_back(inMatrix[y][i]);
			}
			nodenum +=insertValue(baseNode, row, y);
		}
	}
	if(isBenchmarking)
	{
		for(int i = 0; i < 1000; ++i)
		{
			ret =  searchSequenceSearch(baseNode, sequence, accumulatedTime);
		}
		accumulatedTime/=1000;
	}
	else
	{
		ret =  searchSequenceSearch(baseNode, sequence, accumulatedTime);
	}
	deleteNodes(baseNode);
	return ret;
}

std::vector<int> searchUnorderedSearch(std::map<int,std::map<size_t, int >> processedMap, std::vector<int> const & sequence, double & accumulatedTime)
{
	std::vector<int> ret = std::vector<int>();
	Timer currTimer = Timer();
	std::map<size_t,int> rowScore = std::map<size_t,int>();
	for(int it : sequence)
	{
		std::map<int,std::map<size_t, int >>::iterator mapIt = processedMap.find(it);
		//if it exists
		if(mapIt != processedMap.end())
		{
			for ( auto temp : (*mapIt).second)
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
	currTimer.EndTimer();
	accumulatedTime +=  currTimer.GetDuration().count();
}

std::vector<int> searchUnordered(std::vector<std::vector<int>> const & inMatrix, std::vector<int> const & sequence, double & accumulatedTime, bool const &  isBenchmarking)
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

	if(isBenchmarking)
	{
		for(int i = 0; i< 1000; ++i)
		{
			ret = searchUnorderedSearch(processedMap, sequence, accumulatedTime);
		}
		accumulatedTime /=1000;
	}
	else
	{
		ret = searchUnorderedSearch(processedMap, sequence, accumulatedTime);
	}
	
	return ret;
}

std::vector<int> searchBestMatchSearch(std::map<int,std::map<size_t, int >> processedMap, std::vector<int> const & sequence, double & accumulatedTime)
{
	std::vector<int> ret = std::vector<int>();
	Timer currTimer = Timer();
	std::map<size_t,int> rowScore = std::map<size_t,int>();
	for(int it : sequence)
	{
		std::map<int,std::map<size_t, int >>::iterator mapIt = processedMap.find(it);
		//if it exists
		if(mapIt != processedMap.end())
		{
			for ( auto temp : (*mapIt).second)
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
	std::vector<int> rowIndexes = std::vector<int> ();
	int highestScore = 0;
	int retIndex = 0;
	for(auto it : rowScore)
	{
		if(it.second > highestScore)
		{
			retIndex = it.first;	
			highestScore = it.second;
		}
	}
	currTimer.EndTimer();
	accumulatedTime +=  currTimer.GetDuration().count();
	ret.push_back(retIndex);
	return ret;
}

std::vector<int> searchBestMatch(std::vector<std::vector<int>> const & inMatrix, std::vector<int> const & sequence, double & accumulatedTime, bool const &  isBenchmarking)
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
	if(isBenchmarking)
	{
		for(int i = 0; i< 1000; ++i)
		{
			ret = searchBestMatchSearch(processedMap, sequence, accumulatedTime);
		}
		accumulatedTime /=1000;
	}
	else
	{
		ret = searchBestMatchSearch(processedMap, sequence, accumulatedTime);
	}
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
	bool benchmarking = false;
	int startSearchString = 2;
	//If there is an extra command
	std::string commandString = std::string(argv[2]);
	std::transform(commandString.begin(),commandString.end(),commandString.begin(),
				  [](unsigned char c) -> unsigned char { return std::tolower(c); });
	//If the benchmark command is called 
	if(commandString.compare("benchmark") == 0)
	{
		//searchstring starts one more elem down
		++startSearchString;
		benchmarking = true;
	}
	//Initialize search function map
	std::map<std::string,searchFunction> funcMap = std::map<std::string,searchFunction>();
	funcMap["searchsequence"] = searchSequence;
	funcMap["searchunordered"] = searchUnordered;
	funcMap["searchbestmatch"] = searchBestMatch;
	//get searchfunction
	std::string searchString = std::string(argv[startSearchString]);
	std::vector<int> vecSequence = std::vector<int>();
	//Takes in a .txt file
	if(searchString.find(".txt")!= -1)
	{
		std::ifstream searchfs;
		searchfs.open (searchString);
		std::string line;
		if( searchfs.is_open())
		{
			getline(searchfs,searchString, ' ');
			while ( getline (searchfs,line, ' ') )
			{
		  		vecSequence.push_back(std::stoi(line));
			}
			searchfs.close();
		}
	}
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
	*/
	
	if(vecSequence.size() <=0)
	{
		for(int i = ++startSearchString ; i < argc ; ++i)
		{
			vecSequence.push_back(atoi(argv[i]));
		}
	}
	double time = 0;
	std::vector<int> foundRowIndexes;
	foundRowIndexes = funcMap[searchString](readVec,vecSequence,time,benchmarking);
	std::cout << std::fixed << std::showpoint;
	std::cout << std::setprecision(10);
	std::cout << "Time taken : " << time<< " Microseconds "<<std::endl;
	std::cout << "Below are the list of row indexes that match the search" <<std::endl;
	printIntVec(foundRowIndexes);
	return 0;
}
