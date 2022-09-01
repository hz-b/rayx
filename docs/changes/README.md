# How to create release

- does dev testsuite run successfully?
- merge dev into master
- archive old lastChanges.md file according to semver 
- replace contents of lastChanges.md with new changes
- create new commit ("Release commit") and DON'T forget to tag the commit (SemVer)
- push to master