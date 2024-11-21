import logo from './Colored_neural_network.png';
// src/App.js
import React, { useState } from 'react';
import Canvas from './Canvas';

function App() {

  const [grayscaleImageSrc, setGrayscaleImageSrc] = useState(null);

  const saveImage = (imageData) => {
    // Create an off-screen canvas for resizing
    const offScreenCanvas = document.createElement('canvas');
    offScreenCanvas.width = 28;
    offScreenCanvas.height = 28;
    const ctx = offScreenCanvas.getContext('2d');

    // Draw the captured image onto the off-screen canvas, resizing it
    ctx.drawImage(
      imageDataToCanvas(imageData),
      0,
      0,
      imageData.width,
      imageData.height,
      0,
      0,
      28,
      28
    );

    // Get the resized image data
    const resizedImageData = ctx.getImageData(0, 0, 28, 28);

    // Convert to grayscale
    const grayscaleData = convertToGrayscale(resizedImageData);

    // Now you have the 28x28 grayscale image data
    console.log(grayscaleData);
    // You can save it, send it to a server, or process it further
    // Create a new canvas to draw the grayscale image
    const grayscaleCanvas = document.createElement('canvas');
    grayscaleCanvas.width = 28;
    grayscaleCanvas.height = 28;
    const grayscaleCtx = grayscaleCanvas.getContext('2d');
    const imageDataGrayscale = grayscaleCtx.createImageData(28, 28);

    // Set the grayscale data to the ImageData object
    for (let i = 0; i < grayscaleData.length; i++) {
      const grayscale = grayscaleData[i];
      imageDataGrayscale.data[i * 4] = grayscale;     // R
      imageDataGrayscale.data[i * 4 + 1] = grayscale; // G
      imageDataGrayscale.data[i * 4 + 2] = grayscale; // B
      imageDataGrayscale.data[i * 4 + 3] = 255;       // A
    };
    grayscaleCtx.putImageData(imageDataGrayscale, 0, 0);

    // Get the data URL to display the image
    const dataURL = grayscaleCanvas.toDataURL();
    setGrayscaleImageSrc(dataURL);
  };

  const imageDataToCanvas = (imageData) => {
    const tempCanvas = document.createElement('canvas');
    tempCanvas.width = imageData.width;
    tempCanvas.height = imageData.height;
    const tempCtx = tempCanvas.getContext('2d');
    for (let i = 0; i < imageData.data.length; i += 10) {
      for (let j = 0; j < imageData.data.length; j += 10) {
        for (let k = 0; k < 28; k += 1) {
          for (let l = 0; l < 28; l += 1) {
          // my idea for grayscale is to take the total amount of black pixels in the 10x10 area of the image and add that 
          // to the grayscale value of the corresponding pixel in the 28x28 image.
          }
        }
      }
    }
    tempCtx.putImageData(imageData, 0, 0);
    return tempCanvas;
  };

  const convertToGrayscale = (imageData) => {
    const grayscaleArray = [];
    for (let i = 0; i < imageData.data.length; i += 4) {
      // Get the RGB values
      const r = imageData.data[i];
      const g = imageData.data[i + 1];
      const b = imageData.data[i + 2];

      // Convert to grayscale using luminance formula
      const grayscale = Math.round(0.299 * r + 0.587 * g + 0.114 * b);

      // Push the grayscale value to the array
      grayscaleArray.push(grayscale);
    }
    return grayscaleArray;
  };

  return (
    <div className="App">
      <h1>Draw a Digit</h1>
      <Canvas saveImage={saveImage} />
      {grayscaleImageSrc && (
        <div>
          <h2>28x28 Grayscale Image:</h2>
          <img src={grayscaleImageSrc} alt="Grayscale" />
        </div>
      )}
    </div>
  );
}

export default App;

