#include<iostream>
#include<fstream>
#include<vector>


void printMatrix(std::vector<std::vector<int>> const & inMatrix, std::string const & fileName)
{
	//open outputfilestream
	std::fstream ofs;
	ofs.open (fileName, std::fstream::out | std::fstream::trunc | std::fstream::binary);
	if(ofs.is_open())
	{
		// get row and columns
		int row = inMatrix.size();
		int column = 0;
		if(row > 0)
		{
			column = inMatrix[0].size();
		}
		//first two numbers represent the row and column of the matrix
		ofs.write(reinterpret_cast<char *>(&row),sizeof(row));
		ofs.write(reinterpret_cast<char *>(&column),sizeof(column));
		//print out the rest of the matrix.
		for(std::vector<int> row : inMatrix)
		{
			for(int it : row)
			{
				ofs.write(reinterpret_cast<char *>(&it),sizeof(it));
			}
		}
		//close ofs
		ofs.close();
	}
	else
	{
		std::cout <<" Error opening outputfile handle"<<std::endl;	
	}
}

void printIntVec (std::vector<int> const & inVec)
{
	for (int it : inVec )
	{
		std::cout << it << "  ";	
	}
	std::cout << std::endl;	
}

std::vector<std::vector<int>> readCSVParams(int const & row, int const & column,std::ifstream& ifs)
{
	std::vector<std::vector<int>> ret = std::vector<std::vector<int>>();
	//check if input file stream is open
	if(ifs.is_open())
	{
		std::string line;
		int y =0;
		//while there is a new line and y is smaller than row count
		while(getline(ifs,line) && y< row)
		{
			int it = 0;
			int x = 0;
			std::vector<int> row = std::vector<int>();
			// while it is not at the end of line and x is smaller than the column count
			while(it < line.length() && x <column)
			{
				//get the positition of the comma
				int end = line.find(",",it);
				if(end == -1)
				{
					//if a comma cannot be found skip to the end of the line
					end = line.length();
				}
				//get the string of where the iterator is at
				std::string currentInput = line.substr(it,end - it);
				//push back int
				row.push_back(atoi(currentInput.c_str()));
				//increment the iterator past the comma
				it = end+1;
				//increment x
				++x;
			}
			// push the row into the return matrix
			ret.push_back(row);
			++y;
		}
	}
	else
	{
		std::cout <<"Error" <<std::endl;	
		
	} 
	ifs.close();
	return ret;
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

std::vector<std::vector<int>> CreateRandomMatrix(int const& row, int const& column)
{
	std::vector<std::vector<int>> matrixRet = std::vector<std::vector<int>> ();
	for (int y=0; y < row ; ++y)
	{
		std::vector<int> vecRow = std::vector<int>();
		for(int y =0; y < column;++y)
		{
			vecRow.push_back(std::rand() %1000);
		}
		matrixRet.push_back(vecRow);
	}
	return matrixRet;
}

int main(int argc, char ** argv)
{
	if(argc < 4)
	{
		std::cout <<"Use case : ./matrixgenerator row column filename.dat" <<std::endl;	
		return -1;
	}
	int row = atoi(argv[1]);
	int column = atoi(argv[2]);

	if(row ==0 || column == 0)
	{
		std::cout <<"Please specify valid row or column" << std::endl;
		return -1;	
	}
	std::string outputName(argv[3]);
	if(outputName.find(".dat") == -1)
	{
		std::cout <<"Please specify valid .dat file" << std::endl;
		return -1;	
	}
	std::vector<std::vector<int>>matrixRet = std::vector<std::vector<int>>();
	//if there's 5th option
	if(argc > 4)
	{
		//Check for csv
		std::string check (argv[4]);

		if(check.find(".csv") == -1)
		{
			std::cout <<"Please specify a csv file" << std::endl;
			return -1;	
		}
		std::ifstream fs;
		fs.open (argv[4], std::fstream::app);
		if( !fs.is_open())
		{
			std::cout <<"Please specify a csv file" << std::endl;
			return -1;	
		}
		matrixRet = readCSVParams(row,column,fs);
	}
	else
	{
		matrixRet = CreateRandomMatrix(row,column);
	}
	//12345 is the key for the xorcipher
	matrixRet = XORCipherTwoDimension(matrixRet,12345);	
	printMatrix(matrixRet, outputName);
	return 0;
}
