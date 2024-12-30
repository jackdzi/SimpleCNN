import React, { useRef, useState, useEffect } from 'react';

const Canvas: React.FC = () => {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [pixelData, setPixelData] = useState<number[][]>(
    Array.from({ length: 280 }, () => Array(280).fill(0))
  );
  const [numberData, setNumberData] = useState<number[][]>(
    Array.from({ length: 28 }, () => Array(28).fill(0))
  );
  const [isDrawing, setIsDrawing] = useState(false);

  useEffect(() => {
    const canvas = canvasRef.current;
    const context = canvas?.getContext('2d');

    if (canvas && context) {
      canvas.width = 280;
      canvas.height = 280;
      context.fillStyle = 'white';
      context.fillRect(0, 0, canvas.width, canvas.height);
    }
  }, []);

  const drawPixel = (x: number, y: number) => {
    const context = canvasRef.current?.getContext('2d');

    if (context) {
      context.fillStyle = 'black';
      context.fillRect(x, y, 10, 10);

      const newPixelData = [...pixelData];
      newPixelData[Math.floor(y / 10)][Math.floor(x / 10)] = 1;
      setPixelData(newPixelData);
      updateNumberData(newPixelData);
    }
  };

  const updateNumberData = (data: number[][]) => {
    const newNumberData = Array.from({ length: 28 }, (_, rowIndex) =>
      Array.from({ length: 28 }, (_, colIndex) => {
        let sum = 0;
        for (let i = 0; i < 10; i++) {
          for (let j = 0; j < 10; j++) {
            sum += data[rowIndex * 10 + i][colIndex * 10 + j];
          }
        }
        return Math.round(sum / 100);
      })
    );
    console.log(newNumberData)
    setNumberData(newNumberData);
  };

  const handleMouseDown = (event: React.MouseEvent) => {
    const rect = canvasRef.current?.getBoundingClientRect();
    if (rect) {
      const x = Math.floor(event.clientX - rect.left);
      const y = Math.floor(event.clientY - rect.top);
      drawPixel(x, y);
      setIsDrawing(true);
    }
  };

  const handleMouseMove = (event: React.MouseEvent) => {
    if (!isDrawing) return;
    const rect = canvasRef.current?.getBoundingClientRect();
    if (rect) {
      const x = Math.floor(event.clientX - rect.left);
      const y = Math.floor(event.clientY - rect.top);
      drawPixel(x, y);
    }
  };

  const handleMouseUp = () => {
    setIsDrawing(false);
  };

  return (
    <canvas
      ref={canvasRef}
      style={{ border: '1px solid black' }}
      onMouseDown={handleMouseDown}
      onMouseMove={handleMouseMove}
      onMouseUp={handleMouseUp}
      onMouseLeave={handleMouseUp}
    />
  );
};

export default Canvas;
