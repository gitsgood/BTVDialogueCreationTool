#include <iostream>
#include <fstream>
#include <string> 
#include <filesystem>
#include <vector>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
char getch() {
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    char c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return c;
}
#endif

static void inputSafetyCheckStringToInt(int& outReturnedInt) {
    using namespace std;

    constexpr int INTMIN_32 = -2147483648;
    constexpr int INTMAX_32 = 2147483647;

    string userInput{};
    cout << "Please, input thine integer...\n";
    while (true) {
        long long extractedInt{ 0 };
        int signInt{ 1 };
        int signOffset{ 0 };
        bool invalidInput{ false };

        getline(cin, userInput);
        if (userInput.empty()) { cout << "Don't just sit there doing nothing! Try again...\n"; continue; }
        if (userInput[0] == '-') { signInt = -1; signOffset = 1; }
        if (userInput[0] == '+') { signOffset = 1; }
        if (signOffset == 1 && userInput.size() == 1) { cout << "Cheeky little bugger, don't just throw signs at me, INPUT SOMETHING!\n"; continue; }
        if (userInput.size() - signOffset > 10) { cout << "WE CANNOT CONTAIN THIS MANY NUMBERS IN AN INTEGER SPELL\n\tTry again...\n"; continue; }
        for (int i = 0 + signOffset; i < userInput.size(); i++) {
            if ((userInput[i] > '9' || userInput[i] < '0')) {
                cout << "Thine input contains a non digit, this shall not pass! -> '" << userInput[i] << "'\n";
                invalidInput = true;
                continue;
            }
            extractedInt = extractedInt * 10 + (userInput[i] - '0');
        }
        if (invalidInput) { cout << "\tTRY AGAIN YOU RASCAL!\n"; continue; }
        if (extractedInt > INTMAX_32 || extractedInt * signInt < INTMIN_32) {
            cout << "Thine number is an affront to GOD, show some HUMILITY.\n";
            continue;
        }
        outReturnedInt = extractedInt * signInt;
        break;
    }
}

// To shorten the namespace for easier use 
namespace fs = std::filesystem;

void findDirectoryByName(const fs::path& startPath, const std::string& searchString, std::filesystem::path& outDestinationPath) 
{
    try {
        // We use a recursive iterator to go through all subdirectories as well. 
        for (const fs::directory_entry& entry : fs::recursive_directory_iterator(startPath)) {
            // Check if the current entry is a directory 
            if (fs::is_directory(entry.status())) {
                // Get the directory's name as a string and check for the substring 
                if (entry.path().filename().string().find(searchString) != std::string::npos) {
                    std::cout << "Found matching directory: " << fs::absolute(entry.path()) << std::endl;
                    outDestinationPath = fs::absolute(entry.path()).string();
                }
            }
        }
    }
    catch (fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

namespace JsonString
{
    static int NodeCount{ 0 };
    static int EdgeCount{ 0 };

    static std::string NodeJsonString{ "\n\t\t[\n" };
    static std::string EdgeJsonString{ "\n\t\t[\n" };

    static std::vector<std::string> NodeJsonArray;
    static std::vector<std::string> EdgeJsonArray;

    void nodeGeneration(const std::string& inThisNodeId = "", const std::string& inParentEdgeDialogue = "");

    void formatInputForJson(std::string& outInput)
    {
        outInput = "\"" + outInput;
        outInput += "\"";
        outInput += ",";
    }

    void edgeGeneration(const std::string& inThisEdgeId, const std::string& inParentNodeDialogue)
    {
        int EdgeWeAreIn = EdgeCount;
        EdgeCount++;
        bool bWillTheDialogueContinueAfterThis{ 0 };
        std::string EdgeDialogue;

        std::cout << "\n\tWhat shall option number " << EdgeWeAreIn << " in reply to << " << inParentNodeDialogue << " >> consist of?\n";
        getline(std::cin, EdgeDialogue);

        bool valid{ false };
        char key{ 0 };
        while (!valid) {
            std::cout << "\tDoes this option conclude the dialogue?\n"
                << "\t\t[Y]: Yes\t[N]: No\n";

            key = _getch();  // wait for one keypress

            switch (key) {
            case 'Y':
            case 'y':
                std::cout << "\nYou chose YES.\n";
                valid = true;
                break;

            case 'N':
            case 'n':
                std::cout << "\nYou chose NO.\n";
                bWillTheDialogueContinueAfterThis = true;
                valid = true;
                break;

            default:
                std::cout << "\nInvalid key. Please press Y or N.\n\n";
                break;
            }
        }

        std::string NextNodeId;
        if (!bWillTheDialogueContinueAfterThis) NextNodeId = "";
        else
        {
            NextNodeId = "Node" + std::to_string(NodeCount);
        }

        std::string JsonObjectForThisEdge = "\t\t{\n\t\t\t\"EdgeId\": \"" + inThisEdgeId;
        JsonObjectForThisEdge += "\"";
        JsonObjectForThisEdge += ",\n\t\t\t\"PlayerDialogue\": \"";
        JsonObjectForThisEdge += EdgeDialogue;
        JsonObjectForThisEdge += "\"";
        JsonObjectForThisEdge += ",\n\t\t\t\"NextNodeId\": \"";
        JsonObjectForThisEdge += NextNodeId;
        JsonObjectForThisEdge += "\"\n\t\t}";
        EdgeJsonArray.push_back(JsonObjectForThisEdge);

        if (bWillTheDialogueContinueAfterThis) nodeGeneration(NextNodeId, EdgeDialogue);

    }

    void nodeGeneration(const std::string& inThisNodeId, const std::string& inParentEdgeDialogue)
    {
        int NodeWeAreIn = NodeCount;
        NodeCount++;
        int EdgeAmountForThisNode{ 0 };
        std::string NodeDialogue;
        std::string NodeId;

        if (inParentEdgeDialogue.empty()) std::cout << "\nWhat should the NPC say at first?\n";
        else std::cout << "\nWhat should the NPC say in response to " << inParentEdgeDialogue << std::endl;

        getline(std::cin, NodeDialogue);

        if (inThisNodeId.empty()) NodeId = "\"START\"";
        else
        {
            NodeId = "\"" + inThisNodeId;
            NodeId += "\"";
        }

        std::cout << "\nHow many things can the player say in response to: \n" << NodeDialogue << "\n\n";
        inputSafetyCheckStringToInt(EdgeAmountForThisNode);
        

        std::string JsonObjectForThisNode = "\t\t{\n\t\t\t\"NodeId\": " + NodeId;
        JsonObjectForThisNode += ",\n\t\t\t\"NPCDialogue\": \"";
        JsonObjectForThisNode += NodeDialogue;
        JsonObjectForThisNode += "\"";
        JsonObjectForThisNode += ",\n\t\t\t\"OutgoingEdgeIds\": [";
        for (int i = 0; i < EdgeAmountForThisNode; i++)
        {
            std::string EdgeIdName = "Edge" + std::to_string(EdgeCount);
            JsonObjectForThisNode += "\"";
            JsonObjectForThisNode += EdgeIdName;
            if (i != EdgeAmountForThisNode - 1) JsonObjectForThisNode += "\", ";
            else JsonObjectForThisNode += "\"]";

            std::cout << "When the NPC says: << " << NodeDialogue << " >>\n";
            std::cout << "\tPlayer option: " << i+1 << " out of " << EdgeAmountForThisNode << " will now be generated...\n";
            edgeGeneration(EdgeIdName, NodeDialogue);
        }
        JsonObjectForThisNode += "\n\t\t}";
        NodeJsonArray.push_back(JsonObjectForThisNode);

    }

    std::string generateDialogueJsonString()
    {
        using std::cout;
        using std::cin;
        using std::endl;
        using std::string;

        string Input;
        string FinalOutput{ "{\n\t" };

        cout << "\nWhat should the NPC's identifier be?\n\tNote that the NPC's \"NPCId\" variable must be exactly the same.\n";
        string NPCId{ "\"NPCId\": " };
        getline(cin, Input);
        formatInputForJson(Input);
        NPCId += Input;
        FinalOutput += NPCId;
        Input = "";
        FinalOutput += "\n\t\"Nodes\": \n\t[\n";

        nodeGeneration();

        for (int i = 0; i < NodeJsonArray.size(); i++)
        {
            FinalOutput += NodeJsonArray[i];
            if (i != NodeJsonArray.size() - 1) FinalOutput += ",\n";
            else FinalOutput += "\n\t],\n\t\"Edges\":\n\t[\n";
        }
        for (int i = 0; i < EdgeJsonArray.size(); i++)
        {
            FinalOutput += EdgeJsonArray[i];
            if (i != EdgeJsonArray.size() - 1) FinalOutput += ",\n";
            else FinalOutput += "\n\t]\n}";
        }

        return FinalOutput;
    }
}

int main() 
{
    // Define the path to start searching from (e.g., the current directory) 
    fs::path searchPath = "C:/Users/vladi/Documents/GitHub";

    // The string to look for in directory names 
    std::string searchString = "DialogueJSONs";

    // The string containing the final path we're actually looking to work in.
    fs::path destinationPath{ "" };

    std::cout << "Searching for directories containing '" << searchString << "' in '" << fs::absolute(searchPath) << "'..." << std::endl;
    findDirectoryByName(searchPath, searchString, destinationPath);

    if (destinationPath.empty())
    {
        std::cout << "\nI (the program) could not find the destination directory for the dialogue JSON,\nplease make sure you installed a recent branch of the Burn The Village project...\n";
        return -1;
    }

    std::cout << "\nWill now work on the following directory: " << destinationPath << std::endl;

    std::cout << "\nWhat would you like to name your dialogue JSON file?\n";

    std::string fileName;

    std::getline(std::cin, fileName);

    fileName += ".json";

    destinationPath /= fileName;

    std::cout << "\n Creating file: " << destinationPath << std::endl;

    std::fstream dialogueFile(destinationPath, std::ios::out);

    dialogueFile << JsonString::generateDialogueJsonString();

    dialogueFile.close();

    return 0;
}