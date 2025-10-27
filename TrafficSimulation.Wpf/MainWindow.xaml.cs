using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;

namespace TrafficSimulation.Wpf;

public partial class MainWindow : Window
{
    private readonly MainWindowViewModel _viewModel;
    private readonly Dictionary<string, Func<UserControl>> _pageFactory;
    private readonly Dictionary<string, (string Title, string Subtitle)> _pageMetadata;

    public MainWindow()
    {
        InitializeComponent();
        _viewModel = new MainWindowViewModel();
        DataContext = _viewModel;

        _pageFactory = new Dictionary<string, Func<UserControl>>(StringComparer.OrdinalIgnoreCase)
        {
            ["Simulation"] = () => new Views.SimulationPage(),
            ["Settings"] = () => new Views.SettingsPage(),
            ["Help"] = () => new Views.HelpPage(),
            ["About"] = () => new Views.AboutPage()
        };

        _pageMetadata = new Dictionary<string, (string, string)>(StringComparer.OrdinalIgnoreCase)
        {
            ["Simulation"] = ("Live Simulation", "Monitor the intersection flow with the configured agents."),
            ["Settings"] = ("Simulation Setup", "Choose lane topology, signal timing and vehicle profiles."),
            ["Help"] = ("Help & Guides", "Review tutorials and walkthroughs for each component."),
            ["About"] = ("About", "Learn more about the project and contributors.")
        };

        NavigateTo("Simulation");
    }

    private void Navigation_Checked(object sender, RoutedEventArgs e)
    {
        if (sender is not ToggleButton button || button.Tag is not string key)
        {
            return;
        }

        NavigateTo(key);
    }

    private void NavigateTo(string key)
    {
        if (!_pageFactory.TryGetValue(key, out var factory))
        {
            return;
        }

        foreach (var toggle in new[] { SimulationButton, SetupButton, HelpButton, AboutButton })
        {
            toggle.IsChecked = string.Equals(toggle.Tag as string, key, StringComparison.OrdinalIgnoreCase);
        }

        _viewModel.CurrentPage = factory();

        if (_pageMetadata.TryGetValue(key, out var metadata))
        {
            _viewModel.HeaderTitle = metadata.Title;
            _viewModel.HeaderSubtitle = metadata.Subtitle;
        }
    }

    private void StartSimulation_Click(object sender, RoutedEventArgs e)
    {
        NavigateTo("Simulation");
        _viewModel.TriggerSimulationRequested();
    }
}

public sealed class MainWindowViewModel : INotifyPropertyChanged
{
    private object? _currentPage;
    private string _headerTitle = string.Empty;
    private string _headerSubtitle = string.Empty;

    public event PropertyChangedEventHandler? PropertyChanged;
    public event EventHandler? SimulationRequested;

    public object? CurrentPage
    {
        get => _currentPage;
        set
        {
            if (!ReferenceEquals(_currentPage, value))
            {
                _currentPage = value;
                OnPropertyChanged();
            }
        }
    }

    public string HeaderTitle
    {
        get => _headerTitle;
        set
        {
            if (_headerTitle != value)
            {
                _headerTitle = value;
                OnPropertyChanged();
            }
        }
    }

    public string HeaderSubtitle
    {
        get => _headerSubtitle;
        set
        {
            if (_headerSubtitle != value)
            {
                _headerSubtitle = value;
                OnPropertyChanged();
            }
        }
    }

    public void TriggerSimulationRequested()
    {
        SimulationRequested?.Invoke(this, EventArgs.Empty);
    }

    private void OnPropertyChanged([CallerMemberName] string? propertyName = null)
    {
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
    }
}
