import React from "react";

import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  BarElement,
  Title,
  Tooltip,
  Legend,
} from "chart.js";
import { Bar } from "react-chartjs-2";

ChartJS.register(
  CategoryScale,
  LinearScale,
  BarElement,
  Title,
  Tooltip,
  Legend,
);

interface DataDisplayProps {
  children: string;
}

const DataDisplay: React.FC<DataDisplayProps> = ({ children }) => {
  const parseStringToArray = (input: string): number[] => {
    try {
      const cleanedString = input.replace(/^\[|\]$/g, "");
      return cleanedString.split(",").map(Number);
    } catch (error) {
      console.error("Error parsing data:", error);
      return Array(10).fill(0);
    }
  };

  const numericData = parseStringToArray(children);

  const labels = numericData.map((_, index) => `${index}`);

  const data = {
    labels,
    datasets: [
      {
        label: "Probability",
        data: numericData,
        backgroundColor: "rgba(54, 162, 235, 0.6)",
        borderColor: "rgba(54, 162, 235, 1)",
        borderWidth: 1,
      },
    ],
  };

  const options = {
    responsive: true,
    maintainAspectRatio: false,
    plugins: {
      legend: {
        position: "top" as const,
      },
      title: {
        display: true,
        text: "Probability Distribution",
      },
      tooltip: {
        callbacks: {
          label: (context: any) => {
            return `Probability: ${context.raw.toFixed(4)}`;
          },
        },
      },
    },
    scales: {
      y: {
        beginAtZero: true,
        max: 1,
        title: {
          display: true,
          text: "Probability",
        },
        ticks: {
          callback: (value: number) => value.toFixed(2),
        },
      },
      x: {
        title: {
          display: true,
          text: "Number",
        },
      },
    },
  };

  return (
    <div style={{ width: "100%", height: "400px" }}>
      <Bar data={data} options={options as any} />
    </div>
  );
};

export default DataDisplay;
