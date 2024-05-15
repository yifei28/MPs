#include "anagram.h"
#include <iostream>
#include <fstream>
#include <algorithm>

/***
Utilizing a BinaryTree where each node pairs a sorted string key with a list of anagrams, 
Anagram class provides robust functionality to manage and query anagrams dynamically. 
The core functionalities include adding new words, which involves sorting them alphabetically as keys 
and organizing them into lists of anagrams within the tree structure.
AnagramDict supports loading anagrams from a file during initialization, 
thereby populating the binary tree with pre-existing data. 
Methods for saving anagrams back to a file in various traversal orders 
(pre-order, in-order, post-order) are also implemented, allowing for flexible data handling and output.
***/


/***
An AnagramDict constructor. This Constructor takes in a filepath
and then creates a binary tree out of all the words inside the
binary tree.

Input:
std::string filename - the path to the file

Output:
N/A
***/
AnagramDict::AnagramDict(std::string filename)
{
    filename_ = filename;
    std::string word;
    std::string path = "dictionaries/" + filename_;
    std::ifstream file;
    file.open(path.c_str());

    // read all the word in the file and use addWord() to implement the tree
    while (file >> word)
    {
        addWord(word);
    }

    file.close();
}

/***
An AnagramDict member function. This function sorts the given word in
lexicographical order

Input:
std::string word - the word that needs to be sorted

Output:
std::string - the sorted version of the input
***/
std::string AnagramDict::sortWord(std::string word)
{
    std::sort(word.begin(), word.end()); // sort word string
    return word;
}

/***
An AnagramDict member function. This function adds a word to the tree.
If the words sorted version already exists in the tree add it to the linked
list of that node. If the sorted word does not exist in the tree create a new
node and insert it into the tree
Duplicated words should not be added to the tree.
Input:
std::string word - the word that needs to inserted

Output:
N/A
***/
void AnagramDict::addWord(std::string word)
{
    std::string sorted_word = sortWord(word);
    Node<std::string, std::list<std::string> > *temp = tree.find_node(sorted_word);

    // if sorted_word doesn't not exist in the tree
    if (temp == NULL)
    {
        // create new list and stored the current word, insert the node to tree
        std::cout<<"*";
        std::list<std::string> words;
        words.push_back(word);
        temp = new Node<std::string, std::list<std::string> >(sorted_word, words);
        tree.insert_node(tree.getRoot(), temp);
        std::cout<<"*";
    }
    // if sorted_word already exists in the tree
    else
    {
        // check if word already exist in the list
        std::list<std::string> words = temp->getData();
        std::list<std::string>::iterator it;
        int is_found = 0;
        for (it = words.begin();it != words.end(); it++)
        {
            if (*it == word)
            {
                is_found = 1;
                break;
            }
        }

        // push the word iff word is not in the list
        if (is_found == 0)
        {
            words.push_back(word);
            temp->setData(words);
        }
    }
}

/***
An AnagramDict member function. Does a preorder, postorder, or inorder traversal
and then prints out all the anagrams and words.

The output file should be the traversal order of the tree, but only the data on each line.
View on wiki for more information.

Input:
std::string order - The type of order you want to traverse. Can be "pre", "post", "in"

Output:
N/A
***/
void AnagramDict::saveAnagrams(std::string order)
{
    std::list<Node<std::string, std::list<std::string> > > node_list;
    std::string path = "output/" + order + "_" + filename_;
    std::ofstream file;
    file.open(path.c_str());
    if (!file.is_open())
    {
        // create new file
        file.open(path.c_str(), std::ios::out);
    }

    // based on entered order, store nodes of binary tree to list
    // if preorder
    if (order == "pre")
    {
        tree.pre_order(tree.getRoot(), node_list);
    }
    // if postorder
    else if (order == "post")
    {
        tree.post_order(tree.getRoot(), node_list);
    }
    // if inorder
    else if (order == "in")
    {
        tree.in_order(tree.getRoot(), node_list);
    }
    // if invalid
    else
    {
        std::cout << "invalid input" << std::endl;
        file.close();
        return;
    }

    std::list<Node<std::string, std::list<std::string> > >::iterator it_node;
    std::list<std::string>::iterator it_word;
    // iterate list and write words to output file
    for (it_node = node_list.begin(); it_node != node_list.end(); it_node++)
    {
        std::list<std::string> word_list = it_node->getData();
        // iterate the list of words under each nodes
        for (it_word = word_list.begin(); it_word != word_list.end(); it_word++)
        {
            file << (*it_word) << " ";
        }
        file << std::endl;
    }

    file.close();
}