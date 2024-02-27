import subprocess
import re

def get_current_version():
    try:
        completed_process = subprocess.run(["git", "describe", "--tags", "--abbrev=0"], check=True, text=True, stdout=subprocess.PIPE)
        current_version = completed_process.stdout.strip()
    except subprocess.CalledProcessError:
        current_version = "v0.0.0"
    return current_version

def increment_version(version, part='patch'):
    # Remove the 'v' prefix before incrementing
    version = version.lstrip('v')
    major, minor, patch = map(int, version.split('.'))
    if part == 'major':
        major += 1
        minor = 0
        patch = 0
    elif part == 'minor':
        minor += 1
        patch = 0
    elif part == 'patch':
        patch += 1
    # Add the 'v' prefix back to the version number
    return f"v{major}.{minor}.{patch}"

def main():
    current_version = get_current_version()
    print(f"Current version: {current_version}")
    new_version = input(f"Enter new version (default: increment patch to {increment_version(current_version)}): ").strip()
    
    if not new_version:
        new_version = increment_version(current_version)
    else:
        # Update regex to allow 'v' prefix
        if not re.match(r'^v?\d+\.\d+\.\d+$', new_version):
            print("Error: Version must follow vMAJOR.MINOR.PATCH format.")
            return

    doc_updated = input("Have you updated the docs/changes/lastChanges.md file? [y/N]: ").strip().lower()
    if doc_updated not in ('y', 'yes'):
        print("Please update the docs/changes/lastChanges.md file before proceeding.")
        return

    confirmation = input(f"Are you sure you want to tag and commit as {new_version}? [Y/n]: ").strip().lower()
    if confirmation not in ('y', 'yes', ''):
        print("Aborted.")
        return
    
    try:
        subprocess.run(["git", "tag", "-a", new_version, "-m", f"Version {new_version}"], check=True)
        print(f"Successfully tagged with {new_version}")
        # Provide the user with the command to push the tag
        print(f"To push the tag to the remote repository, run:\ngit push origin {new_version}")
    except subprocess.CalledProcessError as e:
        print(f"Error tagging version: {e}")

if __name__ == "__main__":
    main()
