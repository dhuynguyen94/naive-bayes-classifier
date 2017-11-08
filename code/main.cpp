#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <climits>
#include <cmath>
#include <string.h>

using namespace std;

int train_vocab_count[89528][2];
double p_word[89528][2];
long total_word_count_pos = 0, total_word_count_neg = 0;
int alpha = 20; //smoothening factor
float prior = 0.5f;
int predicted_class[25000];
int actual_class[25000];
vector <string> stopwords {'a','about','above','after','again','against','all','am','an','and'
							,'any','are','aren\'t','as','at','be','because','been','before'
							,'being','below','between','both','but','by','can\'t','cannot'
							,'could','couldn\'t','did','didn\'t','do','does','doesn\'t'
							,'doing','don\'t','down','during','each','few','for','from'
							,'further','had','hadn\'t','has','hasn\'t','have','haven\'t'
							,'having','he','he\'d','he\'ll','he\'s','her','here','here\'s'
							,'hers','herself','him','himself','his','how','how\'s','i','i\'d'
							,'i\'ll','i\'m','i\'ve','if','in','into','is','isn\'t','it','it\'s'
							,'its','itself','let\'s','me','more','most','mustn\'t','my','myself'
							,'no','nor','not','of','off','on','once','only','or','other','ought'
							,'our','ours','ourselves','out','over','own','same','shan\'t','she'
							,'she\'d','she\'ll','she\'s','should','shouldn\'t','so','some','such'
							,'than','that','that\'s','the','their','theirs','them','themselves'
							,'then','there','there\'s','these','they','they\'d','they\'ll'
							,'they\'re','they\'ve','this','those','through','to','too','under'
							,'until','u','very','was','wasn\'t','we','we\'d','we\'ll','we\'re'
							,'we\'ve','were','weren\'t','what','what\'s','when','when\'s'
							,'where','where\'s','which','while','who','who\'s','whom','why'
							,'why\'s','with','won\'t','would','wouldn\'t','you','you\'d'
							,'you\'ll','you\'re','you\'ve','your','yours','yourself','yourselves'};
void initialization()
{
	// initializing train_vocab_count array to zero

}

void calculateTotalWordCount()
{
	for(long i = 0 ; i < 89528; i++)
	{
		total_word_count_pos += train_vocab_count[i][0];
		total_word_count_neg += train_vocab_count[i][1];
	}
	cout<<total_word_count_pos<<" "<<total_word_count_neg<<endl;
}

void printVocabArray()
{
	for(long i = 0 ; i < 89528; i++)
		//if(train_vocab_count[i][0]>100 && train_vocab_count[i][1]<100)
		cout << train_vocab_count[i][0] << " " << train_vocab_count[i][1] << endl;
}

void printProbabilityArray()
{
	for(long i = 0 ; i < 89528; i++)
		//if(train_vocab_count[i][0]>100 && train_vocab_count[i][1]<100)
		cout << p_word[i][0] << " " << p_word[i][1] << endl;
}

void printPredictedClasses()
{
	for(long i = 0 ; i < 25000; i++)
		cout<<predicted_class[i]<<endl;
}

void printActualClasses()
{
	int c1 = 0 , c2 = 0;
	for(long i = 0 ; i < 25000; i++)
	{
		if(actual_class[i] == 0) c1++;
		else c2++;
		//cout<<actual_class[i]<<endl;
	}
	cout<<c1<<" "<<c2<<endl;
}

void compareResults()
{
	int pass = 0, total = 25000;
	cout<<"Failed cases\n================\n";
	for(int i = 0 ; i < total; i++)
	{
		if(predicted_class[i] == actual_class[i])
			pass++;
		else
			cout<<i<<endl;
	}
	cout<<"====================\n";
	cout<<"Passed : "<<((double)pass/total)*100<<"%\n";
	cout<<"Failed : "<<((double)total-pass)/total*100<<"%\n";
}

void totalCountofWords()
{
	ifstream infile; 
	infile.open("../data/dataset/train/labeledBow.feat"); 
	string occurence;
	string delimiter = ":";
	int line = 0;
	cout << "Reading from train/labeledBow.feat" << endl;  
	while(true){
		infile >> occurence;
		if(occurence == "@") { line++; continue; }
		if(stoi(occurence) == -1) break; 
		int delim = occurence.find(delimiter);
		int token = stoi(occurence.substr(0, delim));
		int count = stoi(occurence.substr(delim+1, occurence.length()));
		train_vocab_count[token][line/12500] += count;
	}
	printVocabArray();
	infile.close();
}

void calculateProbability()
{
	calculateTotalWordCount();
	for(long i = 1 ; i < 89528; i++)
	{
		p_word[i][0] = (double)(train_vocab_count[i][0] + alpha) / (total_word_count_pos + alpha);
		p_word[i][1] = (double)(train_vocab_count[i][1] + alpha) / (total_word_count_neg + alpha); 
	}
	printProbabilityArray();
}

void generatePredictions()
{
	ifstream infile; 
	infile.open("../data/dataset/test/labeledBow.feat"); 
	string occurence;
	string delimiter = ":";
	int line = -1;
	vector< pair<int,int> > word;
	cout << "Reading from test/labeledBow.feat" << endl;  
	while(true){
		infile >> occurence;
		if(occurence[0] == '@') 
		{
			line++; 
			if(stoi(occurence.substr(1,occurence.length()))<=5) actual_class[line] = 0;
			else actual_class[line] = 1; 
			infile >> occurence;
		}
		else if(occurence[0] == '#') 
		{
			double probability_pos = 1.0d, probability_neg = 1.0d;
			// for(int i = 0; i < word.size(); i++)
			// {
			// 	cout << word[i].first << " " << word[i].second << endl;
			// }
			//cout << " =================================== " <<endl;
			for(int i = 1; i < word.size(); i++)
			{
				//cout << word[i].first << " " << word[i].second << endl;
				//cout << (p_word[word[i].first][0]) << " " << (p_word[word[i].first][1]) << endl;
				// probability_pos *= ((double)pow((p_word[word[i].first][0]), word[i].second));
				// probability_neg *= ((double)pow((p_word[word[i].first][1]), word[i].second));
				probability_pos = probability_pos*(p_word[word[i].first][0]);
				probability_neg = probability_neg*(p_word[word[i].first][1]);
			}
			//cout<< probability_neg << " " << probability_pos << endl;
			word.erase(word.begin(), word.end());
			if(probability_pos/probability_neg >= 1)
				predicted_class[line] = 1;
			else
				predicted_class[line] = 0;
			continue;
		}
		if(stoi(occurence) == -1) break; 
		else
		{
			int delim = occurence.find(delimiter);
			int token = stoi(occurence.substr(0, delim));
			int count = stoi(occurence.substr(delim+1, occurence.length()));
			int flag=0;
			for(int i=0;i<stopwords.size();i++){
				if(strcmp(stringattoken,stopwords[i])==0){
					flag=1;
					break;
				}
			}
			if(flag==0){
				word.push_back(make_pair(token, count));
			}
		}
	}
	printActualClasses();
	printPredictedClasses();
	compareResults();
	infile.close();
}

int main()
{
	totalCountofWords();
	calculateProbability();
	generatePredictions();
	return 0;
}