#include <zmq.hpp>
#include <unistd.h>
#include <termios.h>
#include <string>

static unsigned int gPressedButtons = 0;
static unsigned int gDownButtons = 0;
static termios originalTerm = {0};
static std::string gConsoleText;
#define NUM_COLUMNS 50
#define NUM_ROWS 25
enum Common_Button
{
    BTN_ACTION1,
    BTN_ACTION2,
    BTN_ACTION3,
    BTN_ACTION4,
    BTN_LEFT,
    BTN_RIGHT,
    BTN_UP,
    BTN_DOWN,
    BTN_MORE,
    BTN_QUIT
};

static void RevertTerminal()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTerm);

    printf("%c[?25h", 0x1B); // Show the cursor
}

static bool IsKeyPressed()
{
    fd_set fileDescMask;
    FD_ZERO(&fileDescMask);
    FD_SET(STDIN_FILENO, &fileDescMask);

    timeval timeSpan = {0, 1000};

    // Check file descriptor provided for read, returns number of ready for read file descriptors
    int err = select(1, &fileDescMask, NULL, NULL, &timeSpan);
    assert(err >= 0);
    
    return (err > 0);
}

void Init()
{
    int err = tcgetattr(STDIN_FILENO, &originalTerm);
    assert(err == 0);

    err = atexit(RevertTerminal); // Register for atexit in case we bail and don't call Common_Close
    assert(err == 0);

    termios term = {0};
    err = tcgetattr(STDIN_FILENO, &term);
    assert(err == 0);
    
    term.c_lflag &= ~(ICANON); // Disable special characters, i.e. EOF, EOL, etc
    term.c_lflag &= ~(ECHO);   // Prevent echo of characters
    term.c_cc[VMIN] = 1;       // Specify min number of bytes before a read() can return
    
    err = tcsetattr(STDIN_FILENO, TCSANOW, &term); 
    assert(err == 0);

    printf("%c[?25l", 0x1B); // Hide the cursor
}

int main(int argc, char**argv)
{
    Init();
        
    zmq::context_t context (2);
    zmq::socket_t socket (context, zmq::socket_type::rep);
    socket.bind("tcp://*:5555");

    while(true)
    {
        zmq::message_t request;
        std::string reqStr, repStr;
        unsigned int newButtons = 0;

        socket.recv(request, zmq::recv_flags::none);
        reqStr = request.to_string();

        // Read in key input
        while(IsKeyPressed())
        {
            unsigned int key = getchar();

            if      (key == '1')    newButtons |= (1 << BTN_ACTION1);
            else if (key == '2')    newButtons |= (1 << BTN_ACTION2);
            else if (key == '3')    newButtons |= (1 << BTN_ACTION3);
            else if (key == '4')    newButtons |= (1 << BTN_ACTION4);
            else if (key == 'w')    newButtons |= (1 << BTN_UP);
            else if (key == 'a')    newButtons |= (1 << BTN_LEFT);
            else if (key == 's')    newButtons |= (1 << BTN_DOWN);
            else if (key == 'd')    newButtons |= (1 << BTN_RIGHT);
            else if (key == 32)     newButtons |= (1 << BTN_MORE);
            else if (key == 'q')    newButtons |= (1 << BTN_QUIT);
        }

        gPressedButtons = (gDownButtons ^ newButtons) & newButtons;
        gDownButtons = newButtons;

        /*
            Update the screen
        */
        printf("%c[H", 0x1B);               // Move cursor to home position
        printf("%s", reqStr.c_str()); // Terminal console is already double buffered, so just print
        printf("%c[J", 0x1B);               // Clear the rest of the screen

        const size_t size = sizeof(gPressedButtons);
        zmq::message_t reply(&gPressedButtons, size);
        socket.send(reply, zmq::send_flags::none);
    }

    return 0;
}