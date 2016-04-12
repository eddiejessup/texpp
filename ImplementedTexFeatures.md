# Introduction #
Currently TeXpp implements all TeX syntax rules from TeXbook (chapter 24) except \read. However not all concrete commands are implemented (the most up-to-date statistic about it can be found in texpp/base/base.cc).

TeXpp supports conditionals and macro expansion, however the representation of expanded items in the document tree still needs to be improved.

# Details #

| **Command**     | **Implemented?** |
|:----------------|:-----------------|
| lexer           | yes              |
| context         | yes              |
| grouping        | yes              |
| \end            | yes              |
| \relax          | yes              |
| \par            | yes              |
| \let            | yes              |
| \show           | yes              |
| \showthe        | yes              |
| \message        | yes              |
| \catcode        | yes              |
| \endlinechar    | yes              |
| \newlinechar    | yes              |
| \escapechar     | yes              |
| integer parameters | yes              |
| dimen parameters | yes              |
| glue parameters | yes              |
| muglue parameters | yes              |
| registerdef     | yes              |
| special integers | yes              |
| boxes           | yes              |
| arithmetic      | yes              |
| prefixes        | yes              |
| token lists     | yes              |
| font expressions | yes              |
| fontdimen       | **stub**         |
| expansion       | yes              |
| macros          | yes              |
| conditionals    | yes              |
| file i/o        | **no**           |
| ... (TODO)      | **no**           |