/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    './index.html',
    './renderer/**/*.{js,html}'
  ],
  darkMode: ['class', "[data-theme='dark']"],
  theme: {
    extend: {
      colors: {
        // Use CSS variable driven accent color with alpha support
        accent: 'rgb(var(--accent) / <alpha-value>)',
        glass: {
          bg: 'rgba(255, 255, 255, 0.1)',
          'bg-strong': 'rgba(255, 255, 255, 0.15)',
          border: 'rgba(255, 255, 255, 0.2)',
        }
      },
      fontFamily: {
        sans: [
          'Poppins',
          'ui-sans-serif', 'system-ui', 'Segoe UI', 'Helvetica', 'Arial', 'Noto Sans', 'Apple Color Emoji', 'Segoe UI Emoji'
        ]
      },
      borderRadius: {
        md: '12px',
        lg: '16px',
        xl: '20px',
        '2xl': '24px'
      },
      boxShadow: {
        card: '0 8px 24px rgba(0,0,0,.08)',
        glass: '0 8px 32px rgba(0, 0, 0, 0.12)',
        glow: '0 0 40px rgba(59, 130, 246, 0.3)'
      },
      backdropBlur: {
        glass: '12px'
      },
      animation: {
        'float': 'float 6s ease-in-out infinite',
        'blob': 'blob 20s ease-in-out infinite',
        'shimmer': 'shimmer 2s ease-in-out infinite',
        'wobble': 'wobble 0.3s ease-in-out',
        'bounce-in': 'bounceIn 0.4s cubic-bezier(0.68, -0.55, 0.265, 1.55)'
      },
      keyframes: {
        float: {
          '0%, 100%': { transform: 'translateY(0px)' },
          '50%': { transform: 'translateY(-10px)' }
        },
        blob: {
          '0%, 100%': { transform: 'translateX(0px) scale(1)' },
          '33%': { transform: 'translateX(30px) scale(1.1)' },
          '66%': { transform: 'translateX(-20px) scale(0.9)' }
        },
        shimmer: {
          '0%': { backgroundPosition: '-200% 0' },
          '100%': { backgroundPosition: '200% 0' }
        },
        wobble: {
          '0%': { transform: 'translateX(0px) rotate(0deg)' },
          '25%': { transform: 'translateX(-5px) rotate(-5deg)' },
          '50%': { transform: 'translateX(5px) rotate(5deg)' },
          '75%': { transform: 'translateX(-3px) rotate(-3deg)' },
          '100%': { transform: 'translateX(0px) rotate(0deg)' }
        },
        bounceIn: {
          '0%': { transform: 'scale(0.3)', opacity: '0' },
          '50%': { transform: 'scale(1.05)' },
          '70%': { transform: 'scale(0.9)' },
          '100%': { transform: 'scale(1)', opacity: '1' }
        }
      }
    }
  },
  plugins: [
    require('@tailwindcss/forms'),
    require('@tailwindcss/typography'),
    require('@tailwindcss/container-queries')
  ],
  safelist: [
    'ring-2','ring-blue-500','bg-blue-50','bg-slate-50','text-blue-600','text-slate-900',
    'scale-95','scale-100','opacity-0','opacity-100','pointer-events-none'
  ]
};
