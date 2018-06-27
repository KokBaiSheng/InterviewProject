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

std::vector<int> searchSequence(std::vector<std::vector<int>> const & inMatrix, std::vector<int> const & sequence)
{
	std::vector<int> ret = std::vector<int>();
	std::cout <<"beginning Time" <<std::endl;
	const clock_t begin_time = std::clock();
	for (std::vector<int> row : inMatrix)
	{
		bool isSequence = false;
		for(int i = 0; i < row.size(); ++i)
		{
			if(row[i] == sequence[0] && (i - row.size()) >= sequence.size())
			{
				isSequence = true;
				for(int j =0 ; j <sequence.size(); ++j)
				{
					if(row[i+j] != sequence[j])
					{
						isSequence = false;
						break;
					}
				}
				if(isSequence)
				{
					ret.push_back(i);	
					continue;
				}
			}
		}
	}
	std::cout <<"time : " << float(std::clock() - begin_time ) / CLOCKS_PER_SEC <<std::endl;
	return ret;
}

std::vector<int> searchUnordered(std::vector<std::vector<int>> const & inMatrix, std::vector<int> const & sequence)
{
	std::vector<int> ret = std::vector<int>();
	std::map<int,int> basisMap = std::map<int,int>();
	std::cout <<"beginning Time" <<std::endl;
	const clock_t begin_time = std::clock();
	for( int it : sequence)
	{
		// If the key exists increment it
		if(basisMap.count(it))
		{
			++basisMap[it];	
		}
		else // if not set it to one
		{
			basisMap[it] = 1;	
		}
	}
	for (int y = 0; y< inMatrix.size();++y)
	{
		std::map<int,int> checkList = std::map<int,int>(basisMap);
		for(int i = 0; i < inMatrix[y].size(); ++i)
		{
			//if the element exists
			if(checkList.count(inMatrix[y][i]))
			{
				--checkList[inMatrix[y][i]];	
			}
		}
		bool isUnordered = true;
		for(std::pair<const int, int> it : checkList)
		{
			if(it.second!=0)
			{
				isUnordered = false;	
			}
		}
		if(isUnordered)
		{
			ret.push_back(y);	
		}
	}
	std::cout <<"time : " << float(std::clock() - begin_time ) / CLOCKS_PER_SEC <<std::endl;
	return ret;
}

std::vector<int> searchBestMatch(std::vector<std::vector<int>> const & inMatrix, std::vector<int> const & sequence)
{
	std::vector<int> ret = std::vector<int>();
	std::map<int,int> basisMap = std::map<int,int>();
	std::cout <<"beginning Time" <<std::endl;
	const clock_t begin_time = std::clock();
	for( int it : sequence)
	{
		// If the key exists increment it
		if(basisMap.count(it))
		{
			++basisMap[it];	
		}
		else // if not set it to one
		{
			basisMap[it] = 1;	
		}
	}
	std::vector<int> scores = std::vector<int>();
	for (int y = 0; y< inMatrix.size();++y)
	{
		scores.push_back(0);
		std::map<int,int> checkList = std::map<int,int>(basisMap);
		for(int i = 0; i < inMatrix[y].size(); ++i)
		{
			//if the element exists
			if(checkList.count(inMatrix[y][i]))
			{
				--checkList[inMatrix[y][i]];
				++scores[y];
			}
		}
	}
	int highestScore = 0;
	int highestIndex = 0;
	for(int i = 0; i < scores.size();++i)
	{
		if(scores[i] > highestScore)
		{
			highestScore = scores[i];
			highestIndex = i;
		}
	}
	ret.push_back(highestIndex);
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
