================================================================================
README: NCHLT-inlang pronunciation dictionaries
================================================================================


Full name:    The NCHLT within language pronunciation dictionaries for South
              African languages


Description:  Broad phonemic transcriptions for 15k generic words in each of 
              11 languages. Each dictionary has an associated rule set for
              generating pronunciations for unseen words.


Version:      v1.0
Size:         15,000 words
Languages:    Afrikaans         (afr)
              English           (eng)
              isiNdebele        (nbl)
              isiXhosa          (xho)
              isiZulu           (zul)
              Setswana          (tsn)
              Sesotho sa Leboa  (nso)
              Sesotho           (sot)
              siSwati           (ssw)
              Tshivenda         (ven)
              Xitsonga          (tso)


Source:       http://http://rma.nwu.ac.za/. 
              Any queries with regard to updated versions can be directed to
              rma@nwu.ac.za.


==============================================================================


This data is shared under the Creative Commons Attribution 3.0 Unported
(CC BY 3.0) license. For more information see LICENSE.txt


When using any of these dictionaries, please cite:


Marelie Davel, Willem Basson, Charl van Heerden and Etienne Barnard, “NCHLT 
Dictionaries: Project Report”, Technical report, North-West University, 
May 2013, available from https://sites.google.com/site/nchltspeechcorpus/home.


==============================================================================


DESCRIPTION:


Each dictionary contains words considered to be ‘within language’. The aim was
to remove all proper names, code-switched words or spelling errors; a task 
performed using a combination of automated means (spell-checkers and corpus 
frequency counts) as well as manual verification by language practitioners.


For all languages, initial dictionaries were created using existing resources, 
and these then verified by language practitioners. This verification was either
performed at the pronunciation rule level (for the Southern Bantu languages) or 
at the individual pronunciation level (for English and Afrikaans).
This process is described in more detail in the project report. 


==============================================================================


FORMAT:


1. Dictionary: 
   <tab>-separated word and pronunciation per line
   pronunciations are specified using the X-SAMPA phone set
 


2. Rule sets:
   All the files necessary to predict Default&Refine pronunciations:
   nchlt_<language>.rules
   nchlt_<language>.gnulls
   nchlt_<language>.map.graphs
   nchlt_<language>.map.phones


   The pronunciation of a list of unknown words can be created using:
   perl pron_predict.pl <word_list> <language> <dictionary>
   This creates a new <dictionary> matching the words in <word_list>; 
   where <word_list> is expected to be a list of (UTF8-encoded) words, 
   one word per line.


==============================================================================


ADDITIONAL DOCUMENTATION:


1. NCHLT phone set: 
   Available from https://sites.google.com/site/nchltspeechcorpus/home


2. A description of the NCHLT speech corpus: 


   E. Barnard, M. H. Davel, C. van Heerden, F. de Wet and J. Badenhorst, "The 
   NCHLT corpus of the South African languages", in Proc. SLTU, May 2014. 


3. Earlier Afrikaans dictionaries used during development: 


   M. Davel and F. de Wet, "Verifying pronunciation dictionaries using
   conflict analysis", in Proc. Interspeech, Tokyo, Japan, 2010, pp.
   1898–1901.


   W. D. Basson and M. H. Davel, "Comparing grapheme-based and phoneme-based 
   speech recognition for Afrikaans", in Proc. Pattern Recognition Society of 
   South Africa (PRASA), Vanderbijlpark, 2013, pp. 144-148.


4. Earlier English dictionaries used during development:


   L. Loots, M. Davel, E. Barnard and T. Niesler, "Comparing manually-developed
   and data-driven rules for P2P learning", In Proceedings of the 20th annual
   symposium of the Pattern Recognition Society of South Africa (PRASA),
   Stellenbosch, South Africa, Nov 2009.


   R. Mitten, "Computer-usable version of Oxford Advanced Learner's
   Dictionary of Current English. Oxford Text Archive, 1992.


5. All dictionaries for the Southern Bantu languages are based on dictionaries
   described here:


   M. H. Davel and O. Martirosian, "Pronunciation dictionary development in
   resource-scarce environments", Proc. Interspeech, Brighton, UK, 2009, pp.
   2851-2854.


6. Humanly readable pronunciation prediction rules are extracted using 
   Default&Refine: 


   M. Davel and E. Barnard, “Pronunciation prediction with Default&Refine”,
   Computer Speech & Language, volume 22, no 4, pp 374-393, 2008.
  


===============================================================================