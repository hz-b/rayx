## Type of Change

- [ ] Bug fix (non breaking change, fixing an issue)
- [ ] New feature
- [ ] Breaking change (fix or feature that causes existing functionality to not work as expected)
- [ ] Documentation / Wiki update

## Description

_Write a short summary of what this PR does and why. Is it a Breaking Change? Reference relevant issues._

Fixed #issue

**Optional notes for reviewer:**  
_Anything specific you’d like the reviewer to focus on or be aware of?_

----------

## ✅ Pre-Merge Checklist

> [!IMPORTANT]
> By requesting a review, you confirm this PR is complete from your side. Once approved, it may be merged by someone else. Both developers and reviewers must ensure the PR is truly ready for merge when all checks are green.

Please complete each item before requesting a review.

- [ ] Code follows the project's coding standards
- [ ] Unit tests for new functionality are added and pass
- [ ] All existing tests pass
- [ ] Resolved `TODO` Comments (prefer new issues instead)
- [ ] Documentation, if applicable, including:
    - Doxygen comments for any new rayx-core API functions
    - Helpful inline comments where needed for clarity
    - Wiki pages, e.g. updated build instructions, new Element etc.
- [ ] Commits:
    - Use clear and readable commit messages (e.g. [Conventional commits](https://www.conventionalcommits.org/en/v1.0.0/#summary))
    - Squash and rebase onto `master` if individual commits don’t add value
    - Ensure linear commit history (required by `master`)
