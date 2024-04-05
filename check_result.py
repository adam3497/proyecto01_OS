import os
import filecmp

def compare_directories(dir1, dir2):
    print("Comparing files between the directories:")
    print(f"Directory 1: {dir1}")
    print(f"Directory 2: {dir2}\n")

    # Count the number of files and calculate total directory size for both directories
    num_files_dir1, dir1_size = count_files_and_size(dir1)
    num_files_dir2, dir2_size = count_files_and_size(dir2)
    
    print(f"Number of files in {dir1}: {num_files_dir1}")
    print(f"Directory size of {dir1}: {convert_bytes_to_mb(dir1_size)} MB")
    
    print(f"Number of files in {dir2}: {num_files_dir2}")
    print(f"Directory size of {dir2}: {convert_bytes_to_mb(dir2_size)} MB\n")

    different_files = []
    missing_files = []

    # Check for missing files in the result directory compared to the original directory
    for root, _, files in os.walk(dir2):
        for file in files:
            file_path2 = os.path.join(root, file)
            file_path1 = os.path.join(dir1, os.path.relpath(file_path2, dir2))
            
            if not os.path.exists(file_path1):
                print(f"File {os.path.relpath(file_path2, dir2)} exists in {dir2}, but not in {dir1}")
                missing_files.append(os.path.relpath(file_path2, dir2))

    # Iterate through files in the result directory
    for root, _, files in os.walk(dir1):
        for file in files:
            # Construct corresponding file paths in the original directory
            file_path1 = os.path.join(root, file)
            file_path2 = os.path.join(dir2, os.path.relpath(file_path1, dir1))
            
            # Check if the file exists in the original directory
            if os.path.exists(file_path2):
                # Compare files
                if not filecmp.cmp(file_path1, file_path2, shallow=False):
                    print(f"Differences found between {os.path.relpath(file_path1, dir1)} and {os.path.relpath(file_path2, dir2)}")
                    different_files.append((os.path.relpath(file_path1, dir1), os.path.relpath(file_path2, dir2)))
            else:
                print(f"File {os.path.relpath(file_path1, dir1)} exists in {os.path.relpath(dir1, dir2)}, but not in {os.path.relpath(dir2, dir1)}")
                missing_files.append(os.path.relpath(file_path1, dir1))

    if different_files:
        print("\nSummary: The following files are different between the directories:")
        for file_pair in different_files:
            print(f"- {file_pair[0]} (in {dir1}) and {file_pair[1]} (in {dir2})")
    else:
        print("\nSummary: No differences found between the directories.")

    if missing_files:
        print("\nSummary: The following files are missing in the first directory compared to the second one:")
        for missing_file in missing_files:
            print(f"- {missing_file}")
    else:
        print("\nSummary: No missing files found in the first directory compared to the second one.")

def count_files_and_size(directory):
    num_files = 0
    total_size = 0

    for root, _, files in os.walk(directory):
        for file in files:
            file_path = os.path.join(root, file)
            num_files += 1
            total_size += os.path.getsize(file_path)

    return num_files, total_size

def convert_bytes_to_mb(bytes):
    return round(bytes / (1024 * 1024), 2)

# Directories to compare
directory1 = "out/books/"
directory2 = "books/"

# Call the function to compare directories
compare_directories(directory1, directory2)
