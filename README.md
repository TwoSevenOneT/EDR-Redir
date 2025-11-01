### EDR-Redir

EDR-Redir uses a **Bind Filter (mini filter bindflt.sys)** to redirect the Endpoint Detection and Response (EDR) 's working folder to a folder of the attacker's choice.
Alternatively, it can make the folder appear corrupt to prevent the EDR's process services from functioning.

### Command Line Syntax

**EDR-Redir.exe bind `<VirtualPath`> `<BackingPath`>**

_To create bind link from VirtualPath to BackingPath_

**EDR-Redir.exe bind `<VirtualPath`> `<BackingPath`> `<ExceptionPath`>**

_Powerfull mode to create bind link from VirtualPath to BackingPath. Exclude ExceptionPath_

_ExceptionPath often is Antivirus/EDR path. Use this mode when you want to redirect folder like **Program Files, Program Files (x86),...**_

**EDR-Redir.exe bind `<VirtualPath`>**

_To remove a link that was previously created_


## Links

[Using EDR-Redir To Break EDR Via Bind Link and Cloud Filter](https://www.zerosalarium.com/2025/10/DR-Redir-Break-EDR-Via-BindLink-Cloud-Filter.html)

[EDR-Redir V2: Blind EDR With Fake Program Files](https://www.zerosalarium.com/2025/11/EDR-Redir-V2-Blind-EDR-With-Fake-Program-Files.html)

### Some EDR/Antivirus have been successfully tested

- Microsoft Windows Defender
- Elastic Defend
- Sophos Intercept X
- ...

## Demo Video

Youtube EDR-Redir V1: [https://www.youtube.com/watch?v=2_tanx7RSUw](https://www.youtube.com/watch?v=2_tanx7RSUw)

Youtube EDR-Redir V2: [https://youtu.be/PbXPChdWy3E](https://youtu.be/PbXPChdWy3E)

## ☕ Like what I do? You can fuel my creativity with a coffee!

[![Buy Me A Coffee](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://buymeacoffee.com/twosevenonethree)

## Tools For Security Researcher and Hacker

Essential hardware tools that every security researcher and hacker should have in their toolkit:

[Essential Tools For Security Researcher and Hacker](https://www.zerosalarium.com/p/essential-tools-for-security-researcher.html)

## READING

Some books you should read to sharpen your cybersecurity skills, especially in offensive security:

[Books on Programming and Cybersecurity recommended by Zero Salarium Researchers](https://www.zerosalarium.com/2025/10/books-on-programming-and-cybersecurity-recommended.html)

## Author:

[Two Seven One Three](https://x.com/TwoSevenOneT)
