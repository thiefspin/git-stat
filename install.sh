#!/bin/bash

# Git Statistics Utility Installation Script
# Version 1.0

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BINARY_NAME="git-stat"

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to detect OS
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        echo "windows"
    else
        echo "unknown"
    fi
}

# Function to check prerequisites
check_prerequisites() {
    print_status "Checking prerequisites..."

    # Check for C compiler
    if command_exists clang; then
        print_success "Found clang compiler"
        CC="clang"
    elif command_exists gcc; then
        print_success "Found gcc compiler"
        CC="gcc"
    else
        print_error "No C compiler found. Please install clang or gcc."
        exit 1
    fi

    # Check for make
    if command_exists make; then
        print_success "Found make"
    else
        print_error "Make not found. Please install make."
        exit 1
    fi

    # Check for git
    if command_exists git; then
        print_success "Found git"
    else
        print_error "Git not found. Please install git."
        exit 1
    fi
}

# Function to build the project
build_project() {
    print_status "Building git-stat..."

    cd "$SCRIPT_DIR"

    # Clean previous build
    if [ -f "$BINARY_NAME" ]; then
        print_status "Cleaning previous build..."
        make clean
    fi

    # Build
    if CC="$CC" make; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        exit 1
    fi

    # Verify binary exists
    if [ ! -f "$BINARY_NAME" ]; then
        print_error "Binary $BINARY_NAME not found after build"
        exit 1
    fi

    print_success "Binary $BINARY_NAME created"
}

# Function to test the binary
test_binary() {
    print_status "Testing the binary..."

    if ./"$BINARY_NAME" --help >/dev/null 2>&1; then
        print_success "Binary test passed"
    else
        print_error "Binary test failed"
        exit 1
    fi
}

# Function to install system-wide
install_system() {
    print_status "Installing system-wide to /usr/local/bin..."

    if command_exists sudo; then
        if sudo make install; then
            print_success "Installed to /usr/local/bin/$BINARY_NAME"
            return 0
        else
            print_error "System installation failed"
            return 1
        fi
    else
        print_error "sudo not available for system installation"
        return 1
    fi
}

# Function to install for user
install_user() {
    print_status "Installing for current user to ~/.local/bin..."

    # Create directory if it doesn't exist
    mkdir -p "$HOME/.local/bin"

    if make install-user; then
        print_success "Installed to ~/.local/bin/$BINARY_NAME"

        # Check if ~/.local/bin is in PATH
        if [[ ":$PATH:" != *":$HOME/.local/bin:"* ]]; then
            print_warning "~/.local/bin is not in your PATH"
            print_warning "Add the following line to your shell profile (~/.bashrc, ~/.zshrc, etc.):"
            echo "export PATH=\"\$HOME/.local/bin:\$PATH\""
        fi
        return 0
    else
        print_error "User installation failed"
        return 1
    fi
}

# Function to show usage
show_usage() {
    echo "Git Statistics Utility Installation Script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --system        Install system-wide (requires sudo)"
    echo "  --user          Install for current user only"
    echo "  --build-only    Only build, don't install"
    echo "  --help          Show this help message"
    echo ""
    echo "If no option is specified, the script will:"
    echo "1. Try system installation first"
    echo "2. Fall back to user installation if system fails"
}

# Main installation function
main() {
    echo "=================================="
    echo "Git Statistics Utility Installer"
    echo "=================================="
    echo ""

    OS=$(detect_os)
    print_status "Detected OS: $OS"

    # Parse command line arguments
    case "$1" in
        --help|-h)
            show_usage
            exit 0
            ;;
        --system)
            INSTALL_MODE="system"
            ;;
        --user)
            INSTALL_MODE="user"
            ;;
        --build-only)
            INSTALL_MODE="build"
            ;;
        "")
            INSTALL_MODE="auto"
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac

    # Check prerequisites
    check_prerequisites

    # Build the project
    build_project

    # Test the binary
    test_binary

    # Install based on mode
    case "$INSTALL_MODE" in
        system)
            if install_system; then
                print_success "System installation completed"
            else
                exit 1
            fi
            ;;
        user)
            if install_user; then
                print_success "User installation completed"
            else
                exit 1
            fi
            ;;
        build)
            print_success "Build completed. Binary available at: $SCRIPT_DIR/$BINARY_NAME"
            ;;
        auto)
            print_status "Attempting automatic installation..."
            if install_system; then
                print_success "System installation completed"
            elif install_user; then
                print_success "User installation completed"
            else
                print_error "Both system and user installation failed"
                print_status "You can run the binary directly from: $SCRIPT_DIR/$BINARY_NAME"
                exit 1
            fi
            ;;
    esac

    echo ""
    print_success "Installation complete!"
    echo ""
    print_status "You can now run 'git-stat' from any git repository"
    print_status "Use 'git-stat --help' for usage information"
}

# Run main function with all arguments
main "$@"
