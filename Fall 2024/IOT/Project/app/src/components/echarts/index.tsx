import * as echarts from 'echarts';
import { CSSProperties, useEffect, useRef } from 'react';

const ECharts = (props: { options: echarts.EChartsOption; style?: CSSProperties }) => {
  const { options, style = { width: '100%', height: '100%' } } = props;
  const chartRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (chartRef.current) {
      const myChart = echarts.init(chartRef.current);
      myChart.setOption(options);
    }
  }, [options]);

  return <div ref={chartRef} style={style} />;
};

export default ECharts;
