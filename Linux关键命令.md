---
title: Linux关键命令
tags: 文件系统,网络,进程,磁盘
grammar_cjkRuby: true
---

## 文件系统相关

 1. ==ls---list==:指定要显示列表的目录，也可以是具体的文件。
 
 - -a(all, 列出全部文件，包含隐藏文件)
 - -l(list, 输出格式为长格式，文件的详情)
![enter description here](./images/ls-al.png)
>第一列第一位表示文件的类型
>ｃ：字符文件，ｓ：socket文件，－：普通文件，ｄ：目录文件，ｌ：链接文件
>第一列第二位到第十位表示文件的读写执行权限位，前三位为当前文件所属用户的权限，中间三位为该用户所在的用户组的权限，后三位为其他用户的权限。
>第二列：代表文件或者目录的链接数，即目录或者文件下还包含几个目录或者文件数

下面图片来源于网络，解释的较为清晰：
![enter description here](./images/ls.png)
- -i(inode,  显示文件的inode值)
- -n(--numeric-uid-gid，list numeric user and group IDs)
![enter description here](./images/inode.png)

- 思考:将一个目录的权限设置为rw，是否可以进入目录;将一个目录的权限设置为ｘ，是否可以在该目录下建立新文件？

 2. 文件的创建：==touch filename==;文件夹的创建==mkdir filename==

 3. 设置创建文件的指定权限掩码：==umask nnn== or ==umask u=,g=w,o=rwo==
 >利用umask命令可以指定哪些权限将在新文件的默认权限中被删除。例如，可以使用下面的命令创建掩码，使得组用户的写权限，其他用户的读、写和执行权限都被取消:
 >例如：==umask 002==:所对应的文件和目录创建缺省权限分别为6 6 4和7 7 5
![enter description here](./images/umask.png)
 >==umask (-S)==:查看当前的权限掩码

4. 变更文件（目录）的权限：==chmod==
>常见的两种命令形式：==chmod u+x,o-x filename==; ==chmod 664 filename==

5. 变更文件（目录）的所有者和所属组：==chown  -R new的所属者　filename==
>-R : 对目前目录下的所有档案与子目录进行相同的拥有者变更(即以递回的方式逐个变更)

6. 查看文件内容：==cat/less/more/head/tail filename==
>查看文件的第4行到7行之间的内容:head -n 7 filename | tail -n 4
>==cat >> filename==:向文件后面追加内容

7. 查找文件：==locate where which find==
>==locate==:


8. 其他常见的命令:
>删除：==rm==，一旦删除了一个文件，就无法再恢复它
>拷贝：==cp==;==scp==(远程拷贝)
>移动位置：mv , 也可以重命名。











 
 