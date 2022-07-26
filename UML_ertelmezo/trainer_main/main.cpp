
#include <QCoreApplication>
#include <iostream>
#include "trainer/trainer.hpp"
#include "functions.hpp"
#include "common.hpp"
#include <vector>
#include <cstring>

constexpr bool readAndSaveGestureData = true;
constexpr bool BENCHMARK = false;
/// \brief main
///  commandline arguments syntax:
///  command {train [network_in <fileName>] network_out <fileName>}  |  {  draw [test network_in <fileName>]  }
/// \param argc
/// \param argv
/// \return
/// 
int main(int argc, char *argv[])
{
	///átirányítjuk a hibaüzeneteket egy fileba:
	freopen("trainerErrorLog.txt","a",stderr);
	///pontos idő kiírása a hibafájlba:
	printTimeStampToCerr();
	QCoreApplication qCA(argc, argv);
	bool train = false;
	///ha tesztNetwork igaz, akkor a "network" argumentum utáni argumentum
	/// által meghatározott fájlból betölt egy neurális hálót a program,
	/// és az alakzatok lerajzolásakor a háló segtségével a program megállapítja,
	/// hogy melyik alakzatot rajzoltuk
	std::string networkInFileName = "noFileNameGiven";
	std::string networkOutFileName= "noFileNameGiven";
	
	bool networkInGiven = false;
	bool tesztNetwork = false;
	std::vector<std::string> a;
	///a programindítási parancs argumentumait eltároljuk egy vector-ban:
	for (int i = 1; i < argc; ++i) {
		a.push_back( std::string(argv[i]) );
	}
	///értelmezzük a programindítási argumentumokat és azok alapján beállítjuk a fenti logikai változók értékét:
	if(vFind(a,"-h") || vFind(a,"--help") ){
		///illetve ebben az esetben kiírjuk a help-et
		printHelp();
		return 1;
	}
	if( vFind(a,"train")  &&  vFind(a, "draw")){
		std::cerr << "ERROR: both 'train' and 'draw' commandline arguments present, which have opposite meaning!" << std::endl;
		return 1;
	}
	if( !vFind(a, "train")  &&  !vFind(a, "draw")){
		std::cerr << "ERROR: NEITHER 'train' NOR 'draw' commandline arguments present. choose one!";
		return 1;
	}
	if(vFind(a, "train")){
		///a network_in <inFile> paraméterek OPCIONÁLISAK 'train' megadása esetén,
		/// de a network_in argumentum megadása esetén a filenevet nem szabad elhagyni:
		if(vFind(a, "network_in")){
			networkInGiven = true;
			auto networkInTagIt = find(a.begin(), a.end(), ("network_in"));
			if( networkInTagIt == --a.end() ){
				std::cout << "ERROR: OPTIONAL network_in parameter given, BUT without a filename to read the network from" << std::endl;
				return 1;
			}
			///a "network_in" tag után kell lennie a neurális hálót tartalmazó file nevévnek:
			++networkInTagIt;
			networkInFileName = *networkInTagIt;
		}
		
		///a network_out <outFile> paraméterek kötelezőek 'train' megadása esetén:
		{
		train = true;
		if(!vFind(a, "network_out")){
			std::cerr << "ERROR: train parameter given without network_out parameter" << std::endl;
			return 1;
		}
		auto networkOutTagIt = find(a.begin(), a.end(), ("network_out"));
		if( networkOutTagIt == --a.end() ){
			std::cerr << "ERROR: network_out parameter given without filename to write the network to" << std::endl;
			return 1;
		}
		///a "network_out" tag után kell lennie a filenévnek, ahova a neurális hálót mentjük:
		++networkOutTagIt;
		networkOutFileName = *networkOutTagIt;
		}
	}
	if(vFind(a, "draw")){
		///ez a resz a másik programban van megvalositva
		std::cerr << "ERROR: this program is only for network training. drawing and testing have been moved to a different program!" << std::endl;
		return 1;
	}
	
	if(train){
		std::string squaresFileName = "noFileNameGiven";
		std::string nonSquaresFileName = "noFileNameGiven";
		
		std::cout << "A program téglalapok felismerését fogja tanulni" << std::endl;
		std::cout << "Adja meg a negyzeteket tartalmazó file nevét!" << std::endl;
		std::cin >> squaresFileName;
		std::cout << "Adja meg a NEM negyzeteket tartalmazó file nevét!" << std::endl;
		std::cin >> nonSquaresFileName;
		uint epochs = 0;
		std::cout << "adja meg a tanitási ciklusok(epochs) számát" << std::endl;
		std::cin >> epochs;
		double learnRate = 0;
		std::cout << "adja meg a tanulási faktort(learnRate)" << std::endl;
		std::cin >> learnRate;
		uint saveFrequency = 1;
		std::cout << "adja meg hány iterációnként(epoch) mentse a program az épp tanuló neurális hálót)" << std::endl;
		std::cin >> saveFrequency;
		double testRatio;
		std::cout << "adja meg a minták hány százaléka legyen test adat(a maradék tanító adat lesz)" << std::endl;
		std::cin >> testRatio;
		int testErrorStallTreshold;
		std::cout << "adja meg a hány sikertelen javítás után álljon le a tanítás(ha egymás után ennél többször sem csökken az egy epoch beli maximális teszt hibák minimuma, akkor a tanulás befejeződik)" << std::endl;
		std::cin >> testErrorStallTreshold;
		assert(testRatio >= 0);
		testRatio /= 100.0;
		assert(saveFrequency>0);
		
		Trainer<Network, SumOfSquaredErrors> t(testErrorStallTreshold, learnRate, networkOutFileName, networkInGiven, networkInFileName, saveFrequency, 3000, testRatio);
		std::vector<uint> hiddenLayerSizes;
		///Csak akkor kell az initNetwork, ha Trainer konstruktorában az _inFileGiven-t hamisra állítottuk,
		/// (mert inFileGiven=true esetén)a networkInFileName nevű fileból  elkészíthető a Trainer által kezelt neurális háló):
		if(!networkInGiven){
			uint hiddenLayersNum = 1;
			std::cout << "adja meg a belső rétegek számát" << std::endl;
			std::cin >> hiddenLayersNum;
			hiddenLayerSizes.resize(hiddenLayersNum);
			for (uint i = 0; i < hiddenLayersNum; ++i) {
				std::cout << "adja meg a(z)" << i+1 << ". belső réteg méretét" << std::endl;
				std::cin >> hiddenLayerSizes[i];
			}
			t.initNetwork(hiddenLayerSizes);
		}
		t.train(epochs);
		return 1;
	}
	else{
		///ez a resz a másik programban van megvalositva
		std::cerr << "ERROR: this program is only for network training. drawing and testing have been moved to a different program!" << std::endl;
		return 1;
	}
	return 1;
}
