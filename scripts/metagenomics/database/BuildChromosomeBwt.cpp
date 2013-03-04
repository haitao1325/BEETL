// tutorial about suffix arrays.
#include <iostream>
#include <seqan/index.h>
#include "Alphabet.hh"
#include "Timer.hh"

using namespace seqan;
using namespace std;

void getFileName( const string& stem, const char code, const int pile,
		  string& fileName )
{
  fileName=stem;
  fileName+='-';
  fileName+=code;
  fileName+='0';
  assert(pile<=9);
  fileName+=(char)(48+pile);
  // cerr << "Made file name " << fileName << endl;
}


int main ( int numArgs, const char* args[] )
{  
  if (numArgs!=3)
    {
      cerr << "Usage: " << args[0] <<  "outputprefix  fileToConvert" << endl;
      exit(EXIT_FAILURE);
    }
  cerr << "makeBWTSkew inputFile " << args[1] <<endl;
  std::fstream fstrm;
  fstrm.open(args[1], ::std::ios_base::in | ::std::ios_base::binary);
  String<char> fasta_tag;
  String<char> fasta_seq;
  //Read the meta-information.
  readMeta(fstrm, fasta_tag, Fasta());
  std::cout <<"Tag: " << fasta_tag << "\n"; //prints "a test file"
  //Read the sequence.
  read(fstrm, fasta_seq, Fasta());
  fstrm.close();
  for (int i(0);i<length(fasta_seq);i++)
    {
      if (strchr("ACGNT",fasta_seq[i])==NULL)
 	{
	  cerr << "Found invalid character " << fasta_seq[i]
	       << " at position " << i << " in file "
	       << args[1] << endl;
	  fasta_seq[i]='N';
 	}
    }
  appendValue(fasta_seq,'$');
  // std::cout <<"Seq: "<< fasta_seq << "\n"; //prints the sequence
  // ModifiedString< ModifiedString< String<char>, ModView< FunctorComplement<char> > >, ModReverse > myMod(fasta_seq);
  String<unsigned> sa;
  ///Build a suffix array using the Skew7 algorithm.
  resize(sa, length(fasta_seq));
  createSuffixArray(sa, fasta_seq, Skew7());
  char lastChar(notInAlphabet), thisChar;
  int fileNum;
  string fileName;
  FILE *pFile(NULL), *pArrayFile(NULL);
  cout<< length(fasta_seq) <<endl;
  for (int i(0);i<length(fasta_seq);i++)
    {
      if(sa[i] > length(fasta_seq) -1){
	cerr << "sa bigger " << endl;
      }
      thisChar=fasta_seq[sa[i]];
      if (thisChar!=lastChar)
	{
	  fileNum=whichPile[thisChar];
	  assert(fileNum!=nv);
	  string fileNameStem = "bwt_"+(string)args[2];
	  getFileName(fileNameStem,'B',fileNum,fileName);
	  cerr << "Opening new file " << fileName << endl;
	  if (pFile!=NULL) { fclose(pFile); pFile=NULL; }
	  pFile=fopen(fileName.c_str(),"w");
	  assert(pFile!=NULL);
	  getFileName(fileNameStem,'A',fileNum,fileName);
	  cerr << "Opening new file " << fileName << endl;
	  if (pArrayFile!=NULL) { fclose(pArrayFile); pArrayFile=NULL; }
	  pArrayFile=fopen(fileName.c_str(),"w");
	  assert(pArrayFile!=NULL);
	  lastChar=thisChar;
	}
      thisChar=((sa[i]==0)?'$':fasta_seq[sa[i]-1]);
      fputc(thisChar,pFile);
      fwrite(&sa[i],sizeof(unsigned),1,pArrayFile);
           // cout << fasta_seq[sa[i]] << endl;
    } // ~for i
  cout <<endl;
  if (pFile!=NULL) fclose (pFile);
  if (pArrayFile!=NULL) fclose (pArrayFile);
  // for (int i(0);i<10;i++)
  // cout << fasta_seq[sa[i]] << end
  return 0;
} // ~main
