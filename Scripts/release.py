import subprocess
import re
import os

def get_current_version():
    try:
        completed_process = subprocess.run(["git", "describe", "--tags", "--abbrev=0"], check=True, text=True, stdout=subprocess.PIPE)
        current_version = completed_process.stdout.strip()
    except subprocess.CalledProcessError:
        current_version = "v0.0.0"
    return current_version

def increment_version(version, part='patch'):
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
    return f"v{major}.{minor}.{patch}"

def update_cmake_version(version):
    try:
        root_cmake_path = "CMakeLists.txt"
        with open(root_cmake_path, "r") as file:
            data = file.read()
        data = re.sub(r'project\((\w+) VERSION \d+\.\d+\.\d+\)', f'project(\\1 VERSION {version})', data)
        with open(root_cmake_path, "w") as file:
            file.write(data)
        print(f"Updated {root_cmake_path}")
    except Exception as e:
        print(f"Error updating CMakeLists.txt: {e}")

def main():
    current_version = get_current_version()
    print(f"Current version: {current_version}")
    new_version = input(f"Enter new version (default: increment patch to {increment_version(current_version)}): ").strip()
    
    if not new_version:
        new_version = increment_version(current_version)
    else:
        if not re.match(r'^v?\d+\.\d+\.\d+$', new_version):
            print("Error: Version must follow vMAJOR.MINOR.PATCH format.")
            return

    confirmation = input(f"Have you updated the changelog (CHANGELOG.md)? [Y/n]: ").strip().lower()
    if confirmation not in ('y', 'yes', ''):
        print("Aborted.")
        return

    confirmation = input(f"Are you sure you want to tag and commit as {new_version}? [Y/n]: ").strip().lower()
    if confirmation not in ('y', 'yes', ''):
        print("Aborted.")
        return
    
    try:
        # Update CMakeLists.txt and commit changes
        update_cmake_version(new_version.strip('v'))
        
        # Stage changes
        subprocess.run(["git", "add", "."], check=True)
        
        # Commit changes
        commit_message = f"Update version to {new_version}"
        subprocess.run(["git", "commit", "-m", commit_message], check=True)
        
        # Tag the commit
        subprocess.run(["git", "tag", "-a", new_version, "-m", f"Version {new_version}"], check=True)
        
        print(f"Successfully updated to {new_version}")
        print(f"To push the changes and the tag to the remote repository, run:\ngit push origin {new_version} && git push")
    except subprocess.CalledProcessError as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()
