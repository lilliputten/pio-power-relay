const fs = require('fs');
const path = require('path');

// Lookup folders
const targetDirectories = ['lib', 'src'];

// Find all *.c, *.h, *.cpp, *.hpp files
const filePattern = /\.[ch]p*$/;

targetDirectories.forEach(dir => {
  if (fs.existsSync(dir)) {
    // fs.readdirSync with { recursive: true } requires Node.js v18.17.0 or v20.1.0+
    const files = fs.readdirSync(dir, { recursive: true });

    files.forEach(file => {
      if (filePattern.test(file)) {
        // path.join standardizes slashes across Windows and Unix systems
        console.log(path.join(dir, file).replace(/\\/g, '/'));
      }
    });
  }
});

