# Systems Engineering

The provided material is used in the course [Systems Engineering
(511.047)](https://online.uni-salzburg.at/plus_online/lv.detail?cperson_nr=159563&clvnr=284266)
at the University of Salzburg.

## Assignments

The grade will be derived from the following assignments. Percentage of the grade is denoted
between square brackets.

* Assignment 1 \[10\]: [Hello World / MIPS Instructions / program arguments](01-hello-world)
* Assignment 2 \[30\]: [Scheduling (CPU resource management)](02-scheduling)
* Assignment 3 \[40\]: [Virtual memory (isolation and memory management)](03-virtual-memory)
* Assignment 4 \[20\]: [Inter-process communication (IPC) using shared memory](04-ipc)


## Prerequisites 

The course is hosted on Github and it is required to host the project
repositories on Github too. As a result it is beneficial to have a basic
understanding of `git`.

Suggested reads on `git`:
* [Pro Git book]: An openly available book that covers basic to advanced usage.
* Github specifics are covered in the [Github help].
* Github also provides a [git cheat sheet].

[Pro Git book]: http://git-scm.com/book
[Github help]: https://help.github.com/
[git cheat sheet]: https://training.github.com/kit/downloads/github-git-cheat-sheet.pdf

It is beneficial to have basic knowledge on C/C++. While it is certainly
possible to learn those languages on the way through the assignments, the
assignments are designed (time needed for design an implementation) to 
assume basic knowledge of C/C++.

Throughout all assignments it is suggested to use common Unix utilities, e.g.,
`file`, `objdump`, `readelf`, and so on, and their cross-compiled counterparts
to check and inspect the generated binaries. Description for these utilities can be 
found by using the `man` utility. Example: `man file`.


## Grading and Submission guidelines

Students need 51% of the assignments to pass the course.


### Legal and Ethical Issues

Reading the source code for other operating system kernels, such as Linux or
FreeBSD, is allowed, but do not copy code from them literally. Please cite the
code that inspired your own in your design documentation.

Copying from other students is not allowed and will result in both students
failing the course.


### The actual submission

Submissions are due on the arranged date and need to be tagged with
`assignment-<number>` in the corresponding repository.

Example: As soon as you are finished with assignment 1, please tag the current
state like the following

```sh
git tag assignment-1
git push --tags
```

Note that in case you make changes that should go into the submission (only pre
deadline!) you can also update the tag:

```sh
git tag -f assignment-1
git push --tags --force
```

*Don't forget to push the tags as otherwise there is no way for us to see when
you are finished with an assignment!* When in doubt, double check on the Github
webpage that your changes are committed and pushed.


## Repository management

For organizatorial reasons the provided materials, ranging from assignment documentation
to code, in `mipster-for-students` may update over time.  Students *fork* (
keeping it as upstream; not a Github fork) their repositories from 
`mipster-for-students`.

### For students: Setting up your repository

After cloning the repository assigned to you, just add a branch tracking the upstream 
(`mipster-for-students`) repository.

```sh
git remote add upstream git@github.com:cksystemsteaching/mipster-for-students.git
git fetch upstream # fetching also branch information
git checkout -b upstream upstream/master
git checkout master
```

### For students: Staying up to date

In order to keep *your* repository up to date up to date follow

```sh
git checkout upstream
git pull
git checkout master
git merge upstream
# Solve any merge conflicts that arise.
```

### For teachers: Creating a student repository

Setting up a repository for a student or group can be done the following way.

* Create a  repository named `<repo>` wherever appropriate (e.g., a private teaching organization on Github).
* Follow
```sh
git clone git clone git@github.com:cksystemsteaching/<repo>
cd <repo>
git remote add upstream git@github.com:cksystemsteaching/mipster-for-students.git
git fetch upstream # fetching also branch information
git checkout -b upstream upstream/master
git checkout master
git branch --unset-upstream
git merge upstream # should be a nop
git push -u origin master
```
* Add the students to both `<repo>` *and* `mipster-for-students`.
