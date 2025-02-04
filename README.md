# How to use git to get the latest MG-MixF PPP-AR software?

## Install Git  

- If you use Debian or Ubuntu Linux, you can install `git` directly through a `sudo apt-get install git`, which is very simple.   

- If you use Windows, you can download the installation program directly from [Git's official website](https://git-scm.com/downloads) and then install it according to the `default options`.  After the installation is complete, click the right mouse button in the folder to find `Git Bash` and open the terminal.  

- After the installation is completed, you still need the last step of setting. Enter in the terminal:   
  `git config --global user.name "Your Name"`  
  `git config --global user.email "email@example.com"`  
## Get and Update MG-MixF PPP-AR

- Open the terminal and switch to the directory, then use `git clone` to get the MG-MixF PPP-AR code:    
`git clone https://github.com/SunZhiguo19/MG-MixF-PPP-AR-software`  
The first git clone takes a long time. (It is best **not to modify** the code in the MG-MixF PPP-AR directory, you can **copy to other folder** to modify their code. The MG-MixF PPP-AR directory is **only used to get the latest code**.)  
- If you want to update the code, switch to the MG-MixF PPP-AR directory. Then use the terminal to execute the `git pull` command to update the code.  
- You can use `git log` to view the current version after updating the MG-MixF PPP-AR.  

## Git starter tutorial:


1. Graphics, game learning git: https://learngitbranching.js.org/  
2. Novice tutorial: https://www.runoob.com/git/git-tutorial.html  