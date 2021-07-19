# UA_EI
Repository containing all UA's EI practises (Implement a Search Engine in C++)

# Tokenizador / Tokenizer

Class that segments strings in words with the option to detect special cases 

**Heuristics implemented to detect:** Multiple word (MS-TWO) | URL | e-mail | Acronym | Numbers

# Indexador / Index

From a collection of documents, generate the structures that will be used in the search phase.
Using the hash tables: unordered_map *&&* unordered_set

# Buscador / Search Engine
Search engine implemented from the other classes (Tokenizador && IndexadorHash)

In which the models are used DFR(Deviation From Randomness) *&&* Okapi BM25 to return by console an ordered list of documents (priority_queue) corresponding to the corresponding query

The most important class &#8594; **Current stats:** process 83 questions in 20 sec
