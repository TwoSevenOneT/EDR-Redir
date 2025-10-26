### EDR-Redir

EDR-Redir uses a **Bind Filter (mini filter bindflt.sys)** and the **Windows Cloud Filter API (cldflt.sys)** to redirect the Endpoint Detection and Response (EDR) 's working folder to a folder of the attacker's choice.
Alternatively, it can make the folder appear corrupt to prevent the EDR's process services from functioning.

### Command Line Syntax

**EDR-Redir.exe bind `<VirtualPath`> `<BackingPath`>**

_To create bind link from VirtualPath to BackingPath_

**EDR-Redir.exe bind `<VirtualPath`>**

_To remove a link that was previously created_

**EDR-Redir.exe cloud `<SyncRootPath`> create**

_To register cloud sync root folder_

**EDR-Redir.exe cloud `<SyncRootPath`>**

_To remove a syncroot that was previously created_

## Links

[Using EDR-Redir To Break EDR Via Bind Link and Cloud Filter](https://www.zerosalarium.com/2025/10/DR-Redir-Break-EDR-Via-BindLink-Cloud-Filter.html)

### Some EDR/Antivirus have been successfully tested

- Microsoft Windows Defender
- Elastic Defend
- Sophos Intercept X

## ☕ Like what I do? You can fuel my creativity with a coffee!

[![Buy Me A Coffee](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://buymeacoffee.com/twosevenonethree)

## Fuel the Mission

We’re a team of researchers and developers keeping our lab running, renewing essential software licenses, upgrading aging equipment, and getting coffee to reduce stress. **You can support us by buying a few books from our recommended list**. Every purchase directly funds lab maintenance, software licenses, equipment upgrades, and provides very big morale boosts.

[Books on Programming and Cybersecurity recommended by Zero Salarium Researchers](https://www.zerosalarium.com/2025/10/books-on-programming-and-cybersecurity-recommended.html)

## Author:

[Two Seven One Three](https://x.com/TwoSevenOneT)
