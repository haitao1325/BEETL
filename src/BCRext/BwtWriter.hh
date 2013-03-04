/**
 ** Copyright (c) 2011 Illumina, Inc.
 **
 **
 ** This software is covered by the "Illumina Non-Commercial Use Software
 ** and Source Code License Agreement" and any user of this software or
 ** source file is bound by the terms therein (see accompanying file
 ** Illumina_Non-Commercial_Use_Software_and_Source_Code_License_Agreement.pdf)
 **
 ** This file is part of the BEETL software package.
 **
 ** Citation: Markus J. Bauer, Anthony J. Cox and Giovanna Rosone
 ** Lightweight BWT Construction for Very Large String Collections.
 ** Proceedings of CPM 2011, pp.219-231
 **
 **/

#ifndef INCLUDED_BWTWRITER_HH
#define INCLUDED_BWTWRITER_HH

#include "Alphabet.hh"
#include "Config.hh"
#include "LetterCount.hh"
#include "Tools.hh"
#include "Types.hh"

#include <cstdio>
#include <string>


struct BwtWriterBase
{
    virtual ~BwtWriterBase() {};

    virtual void operator()( const char *p, int numChars ) =0;

    virtual void sendRun( char c, int runLength ) =0;
    virtual void sendRunOfPreExistingData( char c, int runLength, int fileNum, size_t posInRamFile, int remainingRunLength )
    {
        sendRun( c, runLength );
    };
    virtual bool isIncremental()
    {
        return false;
    }
}; // ~BwtWriterBase

struct BwtWriterFile : public BwtWriterBase
{
    BwtWriterFile( const string &fileName );

    virtual ~BwtWriterFile();

    //  virtual void operator()( const char* p, int numChars ) =0;


    //  virtual void sendRun( char c, int runLength ) =0;


    FILE *pFile_;
}; // ~BwtWriterBase



struct BwtWriterASCII : public BwtWriterFile
{
    BwtWriterASCII( const string &fileName );

    virtual ~BwtWriterASCII();

    virtual void operator()( const char *p, int numChars );

    virtual void sendRun( char c, int runLength );



    //  FILE* pFile_;
};


struct BwtWriterRunLength : public BwtWriterFile
{
    BwtWriterRunLength( const string &fileName ):
        BwtWriterFile( fileName ),
        runLength_( 0 ),pBuf_( buf_ ),pBufMax_( buf_+ReadBufferSize ), lastChar_( notInAlphabet )
#ifdef REPORT_COMPRESSION_RATIO
        , charsReceived_( 0 ), bytesWritten_( 0 )
#endif
    {}

    virtual ~BwtWriterRunLength();

    virtual void operator()( const char *p, int numChars );

    void sendChar( char c );

    void encodeRun( char c, uint runLength );

    virtual void sendRun( char c, int runLength );

    uint runLength_;
    uchar buf_[ReadBufferSize];
    uchar *pBuf_;
    const uchar *pBufMax_;
    uchar lastChar_;
#ifdef REPORT_COMPRESSION_RATIO
    LetterCountType charsReceived_;
    LetterCountType bytesWritten_;
#endif
}; // ~BwtWriterRunLength

struct BwtWriterIncrementalRunLength : public BwtWriterFile
{
    BwtWriterIncrementalRunLength( const string &fileName );

    virtual ~BwtWriterIncrementalRunLength();

    virtual void operator()( const char *p, int numChars );

    void terminateLastInsertion();
    void sendChar( unsigned char c, unsigned char metadata );

    void encodeRun( char c, uint runLength );

    virtual void sendRun( char c, int runLength );
    virtual void sendRunOfPreExistingData( char c, int runLength, int fileNum, size_t posInRamFile, int remainingRunLength );
    virtual bool isIncremental()
    {
        return true;
    }

    uint runLength_;
    uchar buf_[ReadBufferSize];
    uchar *pBuf_;
    const uchar *pBufMax_;
    uchar lastChar_;
#ifdef REPORT_COMPRESSION_RATIO
    LetterCountType charsReceived_;
    LetterCountType bytesWritten_;
#endif

private:
    uint fileNum_;
    static uint nextFileNum_;
    uint fileNumInReader_;
    size_t filePosInReader_;
    int remainingRunLengthInReader_;
    bool lastFileReturnNeeded_;
    unsigned char onHoldUntilNextReturn_letter_;
    unsigned char onHoldUntilNextReturn_runLength_;
    unsigned char onHoldUntilNextReturn_metadata_;
}; // ~BwtWriterIncrementalRunLength

// new output module to support huffman encoded output
// migrated & adapted from compression.cpp in Tony's Misc CVS tree
// Tobias, 28/11/11

struct BwtWriterHuffman : public BwtWriterFile
{

    BwtWriterHuffman( const string &fileName ) :
        BwtWriterFile( fileName ),
        bitsUsed_( 0 ), lastChar_( notInAlphabet ), runLength_( 0 ), huffmanBufferPos( 0 )
#ifdef REPORT_COMPRESSION_RATIO
        , charsReceived_( 0 ), bytesWritten_( 0 )
#endif
    {
        soFar_.ull = 0;
        numBuf_.ull = 0;

        for ( int i = 0; i < huffmanWriterBufferSize; i++ )
        {
            symBuf[i] = 0;
        }
    }
    void sendToken( unsigned long long code, uint length );

    virtual void operator()( const char *p, int numChars );

    virtual void sendRun( char c, int runLength );

    void sendNum( uint runLength );

    void emptyBuffer( void );

    void processBuffer( int itemsToPrint );

    virtual ~BwtWriterHuffman(); // ~BwtWriterHuffman()

    BitBuffer soFar_;
    BitBuffer toAdd_;
    BitBuffer numBuf_;
    int bitsUsed_;
    char lastChar_;
    uint runLength_;
    uint huffmanBufferPos;
    uchar symBuf[huffmanWriterBufferSize];

#ifdef REPORT_COMPRESSION_RATIO
    LetterCountType charsReceived_;
    LetterCountType bytesWritten_;
#endif
}; // ~BwtWriterHuffman

struct BwtWriterImplicit : public BwtWriterBase
{
    BwtWriterImplicit( BwtWriterBase *pWriter ) :
        BwtWriterBase(),  pWriter_( pWriter ),
        lastChar_( notInAlphabet ), lastRun_( 0 ), inSAP_( false ) {}

    virtual ~BwtWriterImplicit();

    virtual void operator()( const char *p, int numChars );

    virtual void sendRun( char c, int runLength );

    void flushSAP( void );

    LetterCount countSAP_;
    int firstSAP_;

    BwtWriterBase *pWriter_;

    char lastChar_;
    int lastRun_;


    bool inSAP_;
    //  FILE* pFile_;
}; // ~BwtWriterImplicit : public BwtWriterBase




#endif

