#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <thread>
#include <stdlib.h>     // atoi

#include "constant.h"
#include "connections.h"
#include "Membership.h"
#include "FileSystem.h"

#include "Crane.h"
#include "SpoutTwits.h"
#include "SpoutCalgary.h"
#include "SpoutLawrence.h"

#include "Supervisor.h"
#include "BoltGender.h"
#include "BoltSplitGender.h"
#include "BoltFilterMale.h"
#include "BoltFilterFemale.h"
#include "BoltAddElement.h"

#include "BoltFilterGif.h"
#include "BoltCountHtml.h"
#include "BoltCountJpeg.h"
#include "BoltRankHtml.h"

#include "BoltRankProtocol.h"
#include "BoltRankBytes.h"
#include "BoltCountConnections.h"
#include "BoltSumBytes.h"


using namespace std;

std::ofstream logFile;

/* User thread: Waits for user to input a grep command 
When receiving the grep command from command line (test cases uses this), 
it will bypass the cin*/
void listeningCin(Membership* m, FileSystem* fs, Crane* crane)
{
    std::string input;

    while (true)
    {
        std::cout << "Type a command (help for a list): ";
        // getline(std::cin, input);
        std::cin >> input;
        //std::cout << "You entered: " << input << std::endl;

        if (input.compare("help") == 0 || input.compare("h") == 0)
        {
            std::cout << "quit      (q)     - Exit normally" << std::endl;
            std::cout << "table     (t)     - Table of Membership List" << std::endl;
            std::cout << "join      (j)     - Join the system" << std::endl;
            std::cout << "leave     (l)     - Leave the system" << std::endl;
            std::cout << "leader    (ml)    - Leave the system" << std::endl;
            std::cout << "netstat   (n)     - Network usage" << std::endl;
            std::cout << "vtable    (vt)    - List the virtual ring for the filesystem" << std::endl;
            std::cout << "put       (p)     - Insert a file (put localfile remotefile)" << std::endl;
            std::cout << "get       (g)     - Get a file  (get remotefile localfile)" << std::endl;
            std::cout << "rm        ()      - Remove a file  (rm remotefile)" << std::endl;
            std::cout << "stored    ()      - List files in the present node" << std::endl;
        }
        else if (input.compare("quit") == 0 || input.compare("q") == 0)
        {
            std::cout << "Exiting normally " << std::endl;
            break;
        }
        else if (input.compare("clear") == 0 || input.compare("c") == 0)
        {
            system("clear");
        }
        else if (input.compare("table") == 0 || input.compare("t") == 0)
        {
            std::cout << m->printMember();
        }
        else if (input.compare("leave") == 0 || input.compare("l") == 0)
        {
            m->leave();
        }
        else if (input.compare("join") == 0 || input.compare("j") == 0)
        {
            m->join();
        }
        else if (input.compare("leader") == 0 || input.compare("ml") == 0)
        {
            std::cout << m->getLeader() << std::endl;
        }
        else if (input.compare("netstat") == 0 || input.compare("n") == 0)
        {
            std::cout << "UDP Stats: Sent: " << getUDPSent();
            std::cout << " Received: " << getUDPReceived() << std::endl;
        }
        else if (input.compare("put") == 0 || input.compare("p") == 0)
        {
            std::string localfile;
            std::string remotefile;
            std::cin >> localfile;
            std::cin >> remotefile;

            fs->put(localfile, remotefile);
        }
        else if (input.compare("get") == 0 || input.compare("g") == 0)
        {
            std::string localfile;
            std::string remotefile;
            std::cin >> remotefile;
            std::cin >> localfile;

            fs->get(localfile, remotefile);
        }
        else if (input.compare("vtable") == 0 || input.compare("vt") == 0)
        {
            fs->printVirtualRing();
        }
        else if (input.compare("rm") == 0)
        {
            std::string remotefile;
            std::cin >> remotefile;
            fs->deleteFromFS(remotefile);
        }
        else if (input.compare("list") == 0)
        {
            std::string remotefile;
            std::cin >> remotefile;
            fs->where(remotefile);
        }
        else if (input.compare("stored") == 0 || input.compare("s") == 0)
        {
            std::cout << "Local Files List: " << std::endl;
            std::cout << fs->getFileList() << std::endl;
        }
        else if (input.compare("crane") == 0)
        {
            std::string action;
            std::cin >> action;
            if (crane != NULL)
            {
                if (action.compare("start") == 0)
                {
                    std::cout << "Starting crane... " << std::endl;
                    crane->run();
                }
                else if (action.compare("stop") == 0)
                {
                    std::cout << "Stoping crane... " << std::endl;
                    crane->stop();
                }
            }
        }

        else{
            std::cout << "PLEASE CHECK AGAIN THE POSSIBLE OPTIONS" << std::endl;
        }

    }

    return;
}

int main (int argc, char* argv[])
{
    srand (time(NULL));

    system("rm files/*");
    system("clear");

    int port = atoi(argv[1]);
    bool isIntroducer = atoi(argv[2]);
    int membershipPort = port;
    int filesystemPort = port+21;
    int cranePort      = port+23;

    std::cout << std::endl << "CS425 - MP4: Crane System" ;
    std::cout << std::endl << std::endl;

    logFile.open("log.log");
    logFile << "Inicializing! " << std::endl;

    Membership m(isIntroducer, port);
    FileSystem fs (filesystemPort, m);

    Supervisor* supervisor =  new Supervisor(cranePort);
    supervisor -> run();
    
    // Create Crane
    Crane* crane = NULL;
    if (isIntroducer)
    {
        crane = new Crane(m, cranePort);

        // APP 0 - Filter by gender splitting.  
/*
        SpoutTwits* spout =  new SpoutTwits("spout",1); // Create the spout which generates sentences
        BoltSplitGender* bolt1 =  new BoltSplitGender("bolt1", 3);
        BoltFilterByGender* bolt2 =  new BoltFilterByGender("bolt2", 2);

        crane->addSpout(*spout);
        crane->addBolt(*bolt1); // This will load information about ips.
        crane->addBolt(*bolt2);

        // Spout subscribe bolts
        spout->subscribe(*bolt1);

        //Bolt subscribe itself to other bolts, different from spouts
        bolt2->subscribe(*bolt1, cranePort);
*/

        // APP 1 - Filter by gender against STORM.  30s, 7100 male, 7000 female. 1, 1, 1, 1, 3 machines
/*
        SpoutTwits* spout =  new SpoutTwits("spout",1); // Create the spout which generates sentences
        BoltAddElement* bolt1   =  new BoltAddElement("bolt1", 1);
        BoltFilterMale* bolt2   =  new BoltFilterMale("bolt2", 1);
        BoltFilterFemale* bolt3 =  new BoltFilterFemale("bolt3", 1);

        crane->addSpout(*spout);
        crane->addBolt(*bolt1); // This will load information about ips.
        crane->addBolt(*bolt2);
        crane->addBolt(*bolt3);

        // Spout subscribe bolts
        spout->subscribe(*bolt1);

        //Bolt subscribe itself to other bolts, different from spouts
        bolt2->subscribe(*bolt1, cranePort);
        bolt3->subscribe(*bolt1, cranePort);

        sleep(3);
        std::cout << "Crane Topology succesfully created... " << std::endl;
*/

        // APP 2 - Calgary logs.

        SpoutCalgary* spout =  new SpoutCalgary("spout",1); // Create the spout which generates sentences
        BoltFilterGif* bolt1 =  new BoltFilterGif("bolt1", 1);
        //BoltCountHtml* bolt2 =  new BoltCountHtml("bolt2", 1);
        BoltCountJpeg* bolt3 =  new BoltCountJpeg("bolt3", 1);
        //BoltRankHtml*  bolt4 =  new BoltRankHtml("bolt4", 1);

        crane->addSpout(*spout);
        crane->addBolt(*bolt1); // This will load information about ips.
        //crane->addBolt(*bolt2);
        crane->addBolt(*bolt3);
        //crane->addBolt(*bolt4);

        // Spout subscribe bolts
        spout->subscribe(*bolt1);

        //Bolt subscribe itself to other bolts, different from spouts
        //bolt2->subscribe(*bolt1, cranePort);
        bolt3->subscribe(*bolt1, cranePort);
        //bolt4->subscribe(*bolt2, cranePort);


        SpoutCalgary* spout1 =  new SpoutCalgary("spout1",1); // Create the spout which generates sentences
        BoltFilterGif* bolt11 =  new BoltFilterGif("bolt11", 1);
        BoltCountJpeg* bolt31 =  new BoltCountJpeg("bolt31", 1);
        
        crane->addSpout(*spout1);
        crane->addBolt(*bolt11); // This will load information about ips.
        crane->addBolt(*bolt31);
        
        spout1->subscribe(*bolt11);
        bolt31->subscribe(*bolt11, cranePort);


        sleep(3);
        std::cout << "Crane Topology succesfully created... " << std::endl;


        // APP 3 - Lawrence logs.
/*
        SpoutLawrence* spout =  new SpoutLawrence("spout",1); // Create the spout which generates sentences

        BoltCountConnections* bolt1 =  new BoltCountConnections("bolt1", 1);
        BoltSumBytes* bolt2 =  new BoltSumBytes("bolt2", 1);
        BoltRankProtocol* bolt3 =  new BoltRankProtocol("bolt3", 1);
        BoltRankBytes*  bolt4 =  new BoltRankBytes("bolt4", 1);

        crane->addSpout(*spout);
        crane->addBolt(*bolt1); // This will load information about ips.
        crane->addBolt(*bolt2);
        crane->addBolt(*bolt3);
        crane->addBolt(*bolt4);

        // Spout subscribe bolts
        spout->subscribe(*bolt1);
        spout->subscribe(*bolt2);

        //Bolt subscribe itself to other bolts, different from spouts
        bolt3->subscribe(*bolt1, cranePort);
        bolt4->subscribe(*bolt2, cranePort);

        sleep(3);
        std::cout << "Crane Topology succesfully created... " << std::endl;
*/        
    }

    std::thread cinListening(listeningCin, &m, &fs, crane);


    cinListening.join();

    return 0;
}
