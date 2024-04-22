#include <iostream>
#include <cstring>
int main()
{
    char *msg = strdup("PRIVMSG ola opa oma : hello loooool");
    char *prefix = strtok(msg, " ");// PRIVMSG
    char *target = strtok(NULL, ":"); //OMAR :
    char *text = strtok(NULL, ""); // hello


    if  (!prefix || strcmp(prefix, "PRIVMSG"))
        return 0;
    if (text && text[0] == ' ')
        text++;
    else if (text && text[0] != ' ')
		text = strtok(text, " ");
    std::cout << "DEubg -" << prefix << std::endl;
    std::cout << "DEubg -" << target << std::endl;
    std::cout << "DEubg -" << text << std::endl;
}