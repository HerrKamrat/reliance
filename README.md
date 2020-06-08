# Dependency resolution

## Pub-sub
* On prop change: update all dependees
* Can cause dependees to update multiple times

## Sorting
https://en.wikipedia.org/wiki/Topological_sorting
* Keep sorted list of all properties
* On prop change: count += prop.dependees
* While count > 0: iterate over list 
* What if last prop depends on first? Iterate over whole list?

## Graph/tree
https://www.electricmonk.nl/log/2008/08/07/dependency-resolving-algorithm/git 
* Keep dependency tree of properties
* On prop change: update subtree
