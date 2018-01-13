#!/bin/bash
###########################
####### SETUP FIREWALL ####
###########################
#https://community.vscale.io/hc/ru/community/posts/208348529-Настройка-фаервола-в-Ubuntu-с-помощью-утилиты-UFW?page=1#community_comment_208858805
#https://www.8host.com/blog/nastrojka-brandmauera-ufw-na-servere-ubuntu-16-04/
sudo ufw disable
sudo ufw reset
sudo ufw default deny incoming
sudo ufw default allow outgoing
sudo ufw allow ssh
sudo ufw allow samba
sudo ufw allow postgres
#sudo ufw allow ftp
sudo ufw enable
sudo ufw status numbered
